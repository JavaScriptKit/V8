// swift-tools-version:5.0
import PackageDescription

let package = Package(
    name: "V8",
    products: [
        .library(name: "V8", targets: ["V8"]),
    ],
    dependencies: [
        .package(
            url: "https://github.com/swiftstack/platform.git",
            .branch("dev")),
        .package(
            url: "https://github.com/swiftstack/javascript.git",
            .branch("dev")),
        .package(
            url: "https://github.com/swiftstack/test.git",
            .branch("dev"))
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
