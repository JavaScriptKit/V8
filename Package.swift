// swift-tools-version:4.2
/******************************************************************************
 *                                                                            *
 * Tris Foundation disclaims copyright to this source code.                   *
 * In place of a legal notice, here is a blessing:                            *
 *                                                                            *
 *     May you do good and not evil.                                          *
 *     May you find forgiveness for yourself and forgive others.              *
 *     May you share freely, never taking more than you give.                 *
 *                                                                            *
 ******************************************************************************/

import PackageDescription

let package = Package(
    name: "JavaScript",
    products: [
        .library(name: "JavaScript", targets: ["V8"]),
    ],
    dependencies: [
        .package(
            url: "https://github.com/tris-foundation/platform.git",
            .branch("master")),
        .package(
            url: "https://github.com/tris-foundation/test.git",
            .branch("master"))
    ],
    targets: [
        .target(
            name: "CV8",
            dependencies: []),
        .target(
            name: "V8",
            dependencies: ["CV8", "Platform", "JavaScript"]),
        .target(
            name: "JavaScript",
            dependencies: []),
        .testTarget(
            name: "JavaScriptTests",
            dependencies: ["Test", "V8"]),
    ],
    cxxLanguageStandard: .cxx11
)
