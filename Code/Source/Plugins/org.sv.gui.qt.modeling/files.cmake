set(SRC_CPP_FILES

)

set(INTERNAL_CPP_FILES
    svModelCreate.cxx
    svModelCreateAction.cxx
    svModelLegacyLoadAction.cxx
    svModelLegacySaveAction.cxx
    svSegSelectionWidget.cxx
    svModelEdit.cxx
    svModelingPluginActivator.cxx
)

set(MOC_H_FILES
    src/internal/svModelCreate.h
    src/internal/svModelCreateAction.h
    src/internal/svModelLegacyLoadAction.h
    src/internal/svModelLegacySaveAction.h
    src/internal/svSegSelectionWidget.h
    src/internal/svModelEdit.h
    src/internal/svModelingPluginActivator.h
)

set(UI_FILES
    src/internal/svModelCreate.ui
    src/internal/svSegSelectionWidget.ui
    src/internal/svModelEdit.ui
)

set(CACHED_RESOURCE_FILES
  plugin.xml
  resources/model.png
)

set(QRC_FILES

)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
