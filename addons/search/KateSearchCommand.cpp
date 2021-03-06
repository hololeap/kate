/*   Kate search plugin
 *
 * SPDX-FileCopyrightText: 2020 Kåre Särs <kare.sars@iki.fi>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program in a file called COPYING; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "KateSearchCommand.h"
#include "MatchModel.h"

#include <KLocalizedString>

KateSearchCommand::KateSearchCommand(QObject *parent)
    : KTextEditor::Command(QStringList() << QStringLiteral("grep") << QStringLiteral("newGrep") << QStringLiteral("search") << QStringLiteral("newSearch") << QStringLiteral("pgrep") << QStringLiteral("newPGrep"), parent)
{
}

bool KateSearchCommand::exec(KTextEditor::View * /*view*/, const QString &cmd, QString & /*msg*/, const KTextEditor::Range &)
{
    // create a list of args
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QStringList args(cmd.split(QLatin1Char(' '), QString::KeepEmptyParts));
#else
    QStringList args(cmd.split(QLatin1Char(' '), Qt::KeepEmptyParts));
#endif
    QString command = args.takeFirst();
    QString searchText = args.join(QLatin1Char(' '));

    if (command == QLatin1String("grep") || command == QLatin1String("newGrep")) {
        emit setSearchPlace(MatchModel::Folder);
        emit setCurrentFolder();
        if (command == QLatin1String("newGrep"))
            emit newTab();
    }

    else if (command == QLatin1String("search") || command == QLatin1String("newSearch")) {
        emit setSearchPlace(MatchModel::OpenFiles);
        if (command == QLatin1String("newSearch"))
            emit newTab();
    }

    else if (command == QLatin1String("pgrep") || command == QLatin1String("newPGrep")) {
        emit setSearchPlace(MatchModel::Project);
        if (command == QLatin1String("newPGrep"))
            emit newTab();
    }

    emit setSearchString(searchText);
    emit startSearch();

    return true;
}

bool KateSearchCommand::help(KTextEditor::View * /*view*/, const QString &cmd, QString &msg)
{
    if (cmd.startsWith(QLatin1String("grep"))) {
        msg = i18n("Usage: grep [pattern to search for in folder]");
    } else if (cmd.startsWith(QLatin1String("newGrep"))) {
        msg = i18n("Usage: newGrep [pattern to search for in folder]");
    }

    else if (cmd.startsWith(QLatin1String("search"))) {
        msg = i18n("Usage: search [pattern to search for in open files]");
    } else if (cmd.startsWith(QLatin1String("newSearch"))) {
        msg = i18n("Usage: search [pattern to search for in open files]");
    }

    else if (cmd.startsWith(QLatin1String("pgrep"))) {
        msg = i18n("Usage: pgrep [pattern to search for in current project]");
    } else if (cmd.startsWith(QLatin1String("newPGrep"))) {
        msg = i18n("Usage: newPGrep [pattern to search for in current project]");
    }

    return true;
}

// kate: space-indent on; indent-width 4; replace-tabs on;
