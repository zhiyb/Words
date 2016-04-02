import qbs

QtGuiApplication {
    consoleApplication: false
    cpp.cxxLanguageVersion: "c++11"
    Depends {name: "Qt.widgets"}

    files: [
        "drawing.cpp",
        "drawing.h",
        "main.cpp",
        "mainwindow.cpp",
        "mainwindow.h",
        "structures.cpp",
        "structures.h",
    ]

    Group {
        name: "Miscellaneous"
        files: [
            "README.md",
            "example_vocab.json",
        ]
    }

    Group {     // Properties for the produced executable
        fileTagsFilter: product.type
        qbs.install: true
    }
}
