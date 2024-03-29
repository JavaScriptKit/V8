// swift-tools-version:5.9
import PackageDescription

let package = Package(
    name: "V8",
    platforms: [
        .iOS(.v16),
        .macOS(.v13),
    ],
    products: [
        .library(name: "V8", targets: ["V8"]),
    ],
    dependencies: [
        .package(name: "JavaScript"),
        .package(name: "Platform"),
        .package(name: "Test")
    ],
    targets: [
        .target(
            name: "CV8",
            dependencies: [],
            cSettings: [
                .unsafeFlags(
                    ["-I/opt/homebrew/include"],
                    .when(platforms: [.macOS])
                ),
                .unsafeFlags([
                    "-DV8_ENABLE_SANDBOX",
                    "-DV8_COMPRESS_POINTERS"
                ]),
            ],
            swiftSettings: swift6),
        .target(
            name: "V8",
            dependencies: [
                .target(name: "CV8"),
                .product(name: "Platform", package: "platform"),
                .product(name: "JavaScript", package: "javascript"),
            ],
            swiftSettings: swift6)
    ],
    cxxLanguageStandard: .cxx17
)

let swift6: [SwiftSetting] = [
    .enableUpcomingFeature("ConciseMagicFile"),
    .enableUpcomingFeature("ForwardTrailingClosures"),
    .enableUpcomingFeature("ExistentialAny"),
    .enableUpcomingFeature("StrictConcurrency"),
    .enableUpcomingFeature("ImplicitOpenExistentials"),
    .enableUpcomingFeature("BareSlashRegexLiterals"),
]

// MARK: - tests

testTarget("V8") { test in
    test("JavaScript")
    test("JSValue")
}

func testTarget(_ target: String, task: ((String) -> Void) -> Void) {
    task { test in addTest(target: target, name: test) }
}

func addTest(target: String, name: String) {
    package.targets.append(
        .executableTarget(
            name: "Tests/\(target)/\(name)",
            dependencies: [
                .target(name: "V8"),
                .product(name: "Test", package: "test"),
            ],
            path: "Tests/\(target)/\(name)",
            swiftSettings: swift6))
}

// MARK: - custom package source

#if canImport(ObjectiveC)
import Darwin.C
#else
import Glibc
#endif

extension Package.Dependency {
    enum Source: String {
        case local, remote, github

        static var `default`: Self { .github }

        var baseUrl: String {
            switch self {
            case .local: return "../../swiftstack/"
            case .remote: return "https://swiftstack.io/"
            case .github: return "https://github.com/swiftstack/"
            }
        }

        func url(for name: String) -> String {
            return self == .local
                ? baseUrl + name.lowercased()
                : baseUrl + name.lowercased() + ".git"
        }
    }

    static func package(name: String) -> Package.Dependency {
        guard let pointer = getenv("SWIFTSTACK") else {
            return .package(name: name, source: .default)
        }
        guard let source = Source(rawValue: String(cString: pointer)) else {
            fatalError("Invalid source. Use local, remote or github")
        }
        return .package(name: name, source: source)
    }

    static func package(name: String, source: Source) -> Package.Dependency {
        return source == .local
            ? .package(name: name, path: source.url(for: name))
            : .package(url: source.url(for: name), branch: "dev")
    }
}
