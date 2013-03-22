/*  This file is part of the Kate project.
 *
 *  Copyright (C) 2013 Christoph Cullmann <cullmann@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "katetextfolding.h"
#include "katetextbuffer.h"
#include "katetextrange.h"

namespace Kate {
  
TextFolding::FoldingRange::FoldingRange (TextBuffer &buffer, const KTextEditor::Range &range, FoldingRangeFlags _flags)
  : start (new TextCursor (buffer, range.start(), KTextEditor::MovingCursor::MoveOnInsert))
  , end (new TextCursor (buffer, range.end(), KTextEditor::MovingCursor::MoveOnInsert))
  , parent (0)
  , flags (_flags)
{
}
  
TextFolding::FoldingRange::~FoldingRange ()
{
  /**
   * kill all our data!
   * this will recurse all sub-structures!
   */
  delete start;
  delete end;
  qDeleteAll (nestedRanges);
}
  
TextFolding::TextFolding (TextBuffer &buffer)
  : QObject ()
  , m_buffer (buffer)
{
}

bool TextFolding::newFoldingRange (const KTextEditor::Range &range, FoldingRangeFlags flags)
{
  /**
   * sort out invalid and empty ranges
   * that makes no sense, they will never grow again!
   */
  if (!range.isValid() || range.isEmpty())
    return false;
  
  /**
   * create new folding region that we want to insert
   * this will internally create moving cursors!
   */
  FoldingRange *newRange = new FoldingRange (m_buffer, range, flags);
  
  /**
   * the construction of the text cursors might have invalidated this
   * check and bail out if that happens
   * bail out, too, if it can't be inserted!
   */
  if (    !newRange->start->isValid()
       || !newRange->end->isValid()
       || !insertNewFoldingRange (m_foldingRanges, newRange)) {
    /**
     * cleanup and be done
     */
    delete newRange;
    return false;
  }
  
  /**
   * all went fine, newRange is now registered internally!
   */
  return true;
}

QString TextFolding::debugDump () const
{
  /**
   * dump toplevel ranges recursively
   */
  return debugDump (m_foldingRanges);
}

void TextFolding::debugPrint (const QString &title) const
{
  // print title + content
  printf ("%s\n    %s\n", qPrintable (title), qPrintable(debugDump()));
}

QString TextFolding::debugDump (const TextFolding::FoldingRange::Vector &ranges)
{
  /**
   * dump all ranges recursively
   */
  QString dump;
  Q_FOREACH (FoldingRange *range, ranges) {
    if (!dump.isEmpty())
      dump += " ";
    
    dump += QString ("[%1:%2 %3%4 ").arg (range->start->line()).arg(range->start->column()).arg((range->flags & Persistent) ? "p" : "").arg((range->flags & Folded) ? "f" : "");
    
    /**
     * recurse
     */
    QString inner = debugDump (range->nestedRanges);
    if (!inner.isEmpty())
      dump += inner + " ";
    
    dump += QString ("%1:%2]").arg (range->end->line()).arg(range->end->column());
  }
  return dump;
}

bool TextFolding::insertNewFoldingRange (FoldingRange::Vector &existingRanges, FoldingRange *newRange)
{
  /**
   * kill empty ranges
   * might exist because we removed the text inside a range or cleared buffer
   */
  if (!existingRanges.isEmpty()) {
    /**
     * construct brute force new ranges vector
     * this can be OPTIMIZED!
     */
    FoldingRange::Vector newRanges;
    Q_FOREACH (FoldingRange *range, existingRanges) {
      /**
       * there shall be never invalid cursors!
       */
      Q_ASSERT (range->start->isValid() && range->end->isValid());
      
      /**
       * start <= end!
       */
      Q_ASSERT (range->start->toCursor() <= range->end->toCursor());
      
      /**
       * sort out empty ranges
       * DELETE them, transfer the others
       */
      if (range->start->toCursor() < range->end->toCursor())
        newRanges.push_back (range);
      else
        delete range;
    }
    
    /**
     * overwrite old vector in any case!
     */
    existingRanges = newRanges;
  }
  
  /**
   * existing ranges is non-overlapping and sorted
   * that means now, we can search for lower bound of start of range and upper bound of end of range to
   * find all "overlapping" ranges.
   */
  
  /**
   * first: lower bound of start
   */
  FoldingRange::Vector::iterator lowerBound = qLowerBound (existingRanges.begin(), existingRanges.end(), newRange, compareRangeByStart);
  
  /**
   * second: upper bound of end
   */
  FoldingRange::Vector::iterator upperBound = qUpperBound (existingRanges.begin(), existingRanges.end(), newRange, compareRangeByEnd);

  /**
   * we may need to go one to the left, if not already at the begin, as we might overlap with the one in front of us!
   */
  if ((lowerBound != existingRanges.begin()) && ((*(lowerBound-1))->end->toCursor() > newRange->start->toCursor()))
    --lowerBound;
  
  /**
   * now: first case, we overlap with nothing or hit exactly one range!
   */
  if (lowerBound == upperBound) {
    /**
     * nothing we overlap with?
     * then just insert and be done!
     */
    if ((lowerBound == existingRanges.end()) || (newRange->start->toCursor() >= (*lowerBound)->end->toCursor()) || (newRange->end->toCursor() <= (*lowerBound)->start->toCursor())) {
      existingRanges.insert (lowerBound, newRange);
      return true;
    }
    
    /**
     * we are contained in this one range?
     * then recurse!
     */
    if ((newRange->start->toCursor() >= (*lowerBound)->start->toCursor()) && (newRange->end->toCursor() <= (*lowerBound)->end->toCursor()))
      return insertNewFoldingRange ((*lowerBound)->nestedRanges, newRange);
    
    /**
     * else: we might contain at least this fold, or many more, if this if block is not taken at all
     * use the general code that checks for "we contain stuff" below!
     */
  }
  
  /**
   * check if we contain other folds!
   */
  FoldingRange::Vector::iterator it = lowerBound;
  bool includeUpperBound = false;
  FoldingRange::Vector nestedRanges;
  while (it != existingRanges.end()) {
    /**
     * do we need to take look at upper bound, too?
     * if not break
     */
    if (it == upperBound) {
      if (newRange->end->toCursor() <= (*upperBound)->start->toCursor())
        break;
      else
        includeUpperBound = true;
    }
    
    /**
     * if one region is not contained in the new one, abort!
     * then this is not well nested!
     */
    if (!((newRange->start->toCursor() <= (*it)->start->toCursor()) && (newRange->end->toCursor() >= (*it)->end->toCursor())))
      return false;
    
    /**
     * include into new nested ranges
     */
    nestedRanges.push_back ((*it));
      
    /**
     * end reached
     */
    if (it == upperBound)
      break;
    
    /**
     * else increment
     */
    ++it;
  }
  
  /**
   * if we arrive here, all is nicely nested into our new range
   * remove the contained ones here, insert new range with new nested ranges we already constructed
   */
  it = existingRanges.erase (lowerBound, includeUpperBound ? (upperBound+1) : upperBound);
  existingRanges.insert (it, newRange);
  newRange->nestedRanges = nestedRanges;
  return true;
}

bool TextFolding::compareRangeByStart (FoldingRange *a, FoldingRange *b)
{
  return a->start->toCursor() < b->start->toCursor();
}

bool TextFolding::compareRangeByEnd (FoldingRange *a, FoldingRange *b)
{
  return a->end->toCursor() < b->end->toCursor();
}

}
