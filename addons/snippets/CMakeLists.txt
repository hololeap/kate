find_package(KF5NewStuff ${KF5_DEP_VERSION} QUIET)
set_package_properties(KF5NewStuff PROPERTIES PURPOSE "Required to build the snippets addon")

if(NOT KF5NewStuff_FOUND)
  return()
endif()

add_library(katesnippetsplugin MODULE "")
target_compile_definitions(katesnippetsplugin PRIVATE TRANSLATION_DOMAIN="katesnippetsplugin")

target_link_libraries(
  katesnippetsplugin 
  PRIVATE
    KF5::NewStuff 
    KF5::TextEditor
)

ki18n_wrap_ui(UI_SOURCES snippetview.ui editrepository.ui editsnippet.ui)
target_sources(katesnippetsplugin PRIVATE ${UI_SOURCES})

target_sources(
  katesnippetsplugin
  PRIVATE
    katesnippets.cpp
    katesnippetglobal.cpp
    snippetview.cpp
    snippetstore.cpp
    snippetrepository.cpp
    snippetcompletionmodel.cpp
    snippetcompletionitem.cpp
    snippet.cpp
    editrepository.cpp
    editsnippet.cpp
    plugin.qrc
)

kcoreaddons_desktop_to_json(katesnippetsplugin katesnippetsplugin.desktop)
install(TARGETS katesnippetsplugin DESTINATION ${PLUGIN_INSTALL_DIR}/ktexteditor)
