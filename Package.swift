// swift-tools-version:5.0
import PackageDescription

let package = Package(
    name: "V8",
    products: [
        .library(name: "V8", targets: ["V8"]),
    ],
    dependencies: [
        .package(
            url: "https://github.com/tris-code/platform.git",
            .branch("master")),
        .package(
            url: "https://github.com/tris-code/javascript.git",
            .branch("master")),
        .package(
            url: "https://github.com/tris-code/test.git",
            .branch("master"))
    ],
    targets: [
        .target(
            name: "CV8",
            dependencies: []),
        .target(
            name: "V8",
            dependencies: ["CV8", "Platform", "JavaScript"]),
        .testTarget(
            name: "V8Tests",
            dependencies: ["Test", "V8"]),
    ],
    cxxLanguageStandard: .cxx11
)
