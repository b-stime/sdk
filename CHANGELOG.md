## 2.14.0

### Core libraries

#### `dart:core`

*   The native `DateTime` class now better handles local time around
    daylight saving changes that are not precisely one hour.
    (No change on the Web which uses the JavaScript `Date` object.)

## 2.13.0

### Language

*   **Type aliases** [Non-function type aliases][]: Type aliases (names for
    types introduced via the `typedef` keyword) were previously restricted
    to only introduce names for function types.  In this release, we
    remove this restriction and allow type aliases to name any kind of type.

    ```dart
    import 'dart:convert';

    typedef JsonMap = Map<String, dynamic>;

    JsonMap parseJsonMap(String input) => json.decode(input) as JsonMap;
    ```

    In addition to being usable as type annotations, type aliases that name
    class types can now also be used anywhere that the underlying class could be
    used, allowing type aliases to be used to safely rename existing classes.

    ```dart
    class NewClassName<T> {
       NewClassName.create(T x);
       static NewClassName<T> mkOne<T>(T x) => NewClassName<T>.create(x);
     }
    @Deprecated("Use NewClassName instead")
    typedef OldClassName<T> = NewClassName<T>;

    class LegacyClass extends OldClassName<int> {
      LegacyClass() : super.create(3);
    }
    OldClassName<int> legacyCode() {
      var one = OldClassName.create(1);
      var two = OldClassName.mkOne(2);
      return LegacyClass();
    }
    ```

    The new type alias feature is only available as part of the 2.13 [language
    version](https://dart.dev/guides/language/evolution).  To use this feature,
    you must set the lower bound on the sdk constraint for your package to 2.13
    or greater.

    [Non-function type aliases]: https://github.com/dart-lang/language/blob/master/accepted/2.13/nonfunction-type-aliases/feature-specification.md

### Core libraries

#### `dart:collection`

- The `SplayTreeMap` was changed to allow `null` as key if the `compare`
  function allows it. It now checks that a new key can be used as an
  argument to the `compare` function when the member is added,
  *even if the map is empty* (in which case it just compares the key
  to itself).
- The `SplayTreeSet` was changed to checks that a new element can be used as an
  argument to the `compare` function when the member is added,
  *even if the set is empty* (in which case it just compares the element
  to itself).

#### `dart:developer`

- Added `serverWebSocketUri` property to `ServiceProtocolInfo`.

### Dart VM

### Tools

#### Analyzer

- Static analyses with "error" severity can once again be ignored with
  comments like `// ignore: code` and `// ignore_for_file: code`. To declare
  that certain analysis codes, or codes with certain severities ("error",
  "warning", and "info") cannot be ignored with such comments, list them in
  `analysis_options.yaml`, under the `analyzer` heading, with a new YAML key,
  `cannot-ignore`. For example, to declare that "error" codes and
  `unused_import` cannot be ignored, write the following into
  `analysis_options.yaml`:

  ```yaml
  analyzer:
    cannot-ignore:
      - error
      - unused_import
  ```

#### dart format

*   Correct constructor initializer indentation after `required` named
    parameters.

#### Linter

Updated the Linter to `1.2.1`, which includes:

- improvements to `iterable_contains_unrelated_type` to better support `List`
  content checks.
- fixes to `camel_case_types` and `prefer_mixin` to support non-function
  type aliases.
- fixed `prefer_mixin` to properly make exceptions for `dart.collection`
  legacy mixins.
- new lint: `use_build_context_synchronously` (experimental).
- new lint: `avoid_multiple_declarations_per_line`.
- full library migration to null-safety.
- new lint: `use_if_null_to_convert_nulls_to_bools`.
- new lint: `deprecated_consistency`.
- new lint: `use_named_constants`.
- deprecation of `avoid_as`.

### Other libraries

#### `package:js`

*   **Breaking change:** It is no longer valid to use `String`s that match
    an `@Native` annotation in an `@JS()` annotation for a non-anonymous JS
    interop class. This led to erroneous behavior due to the way interceptors
    work. If you need to work with a native class, prefer `dart:html`, an
    `@anonymous` class, or `js_util`. See issue [#44211][] for more details.

[#44211]: https://github.com/dart-lang/sdk/issues/44211

## 2.12.2 - 2021-03-17

This is a patch release that fixes crashes reported by Flutter 2 users (issue
[flutter/flutter#78167][]).

[flutter/flutter#78167]: https://github.com/flutter/flutter/issues/78167

## 2.12.1 - 2021-03-10

This is a patch release that fixes:

* an unhandled exception in HTTPS connections (issue [#45047][]).
* a typing issue in the typed_data `+` operator (issue [#45140][]).

[#45047]: https://github.com/dart-lang/sdk/issues/45047
[#45140]: https://github.com/dart-lang/sdk/issues/45140

## 2.12.0 - 2021-03-03

### Language

*   **Breaking Change** [Null safety][] is now enabled by default in all code
    that has not opted out. With null safety, types in your code are
    non-nullable by default. Null can only flow into parts of your program where
    you want it. With null safety, your runtime null-dereference bugs turn into
    edit-time analysis errors.

    You can opt out of null safety and preserve your code's previous behavior by
    setting the lower bound of the SDK constraint in your pubspec to 2.11.0 or
    earlier to request an earlier [language version][]. You can opt out
    individual Dart files by adding `// @dart=2.11` to the beginning of the
    file.

    Files that are opted in to null safety may report new compile-time errors.
    Opting in to null safety also gives you access to other new language
    features:

    *   Smarter flow analysis and type promotion
    *   `required` named parameters
    *   `late` variables
    *   The postfix `!` null assertion operator
    *   The `?..` and `?[]` null-aware operators

*   **Breaking Change** [#44660][]: Fixed an implementation bug where `this`
    would sometimes undergo type promotion in extensions.

[null safety]: https://dart.dev/null-safety/understanding-null-safety
[language version]: https://dart.dev/guides/language/evolution#language-versioning
[#44660]: https://github.com/dart-lang/sdk/issues/44660

### Core libraries

#### `dart:async`

*   Add extension method `onError()` on `Future` to allow better typing of error
    callbacks.

#### `dart:collection`

*   Add `UnmodifiableSetView` class, which allows users to guarantee that
    methods that could change underlying `Set` instance can not be invoked.

*   Make it explicit that `LinkedList` compares elements by identity, and update
    `contains()` to take advantage of this.

#### `dart:core`

*   Add `Set.unmodifiable()` constructor, which allows users to create
    unmodifiable `Set` instances.

#### `dart:io`

*   `HttpRequest` now correctly follows HTTP 308 redirects
    (`HttpStatus.permanentRedirect`).

#### `dart:isolate`

*   Add `debugName` positional parameter to `ReceivePort` and `RawReceivePort`
    constructors, a name which can be associated with the port and displayed in
    tooling.

#### `dart:html`

*   `EventStreamSubscription.cancel` has been updated to retain its synchronous
    timing when running in both sound and unsound null safety modes. See issue
    [#44157][] for more details.

[#44157]: https://github.com/dart-lang/sdk/issues/44157

### Dart VM

*   **Breaking Change** [#42312][]: `Dart_WeakPersistentHandle`s no longer
    auto-delete themselves when the referenced object is garbage collected to
    avoid race conditions, but they are still automatically deleted when the
    isolate group shuts down.

*   **Breaking Change** [#42312][]: `Dart_WeakPersistentHandleFinalizer`
    is renamed to `Dart_HandleFinalizer` and had its `handle` argument removed.
    All API functions using that type have been updated.

[#42312]: https://github.com/dart-lang/sdk/issues/42312

### Foreign Function Interface (`dart:ffi`)

*   **Breaking Change** [#44621][]: Invocations with a generic `T` of
    `sizeOf<T>`, `Pointer<T>.elementAt()`, `Pointer<T extends Struct>.ref`, and
    `Pointer<T extends Struct>[]` are being deprecated in the current stable
    release (2.12), and are planned to be fully removed in the following stable
    release (2.13). Consequently, `allocate` in `package:ffi` will no longer be
    able to invoke `sizeOf<T>` generically, and will be deprecated as well.
    Instead, the `Allocator` it is introduced to `dart:ffi`, and also requires
    a constant `T` on invocations. For migration notes see the breaking change
    request.

*   **Breaking Change** [#44622][]: Subtypes of `Struct` without any native
    member are being deprecated in the current stable release (2.12), and are
    planned to be fully removed in the following stable release (2.13).
    Migrate opaque types to extend `Opaque` rather than `Struct`.

[#44621]: https://github.com/dart-lang/sdk/issues/44621
[#44622]: https://github.com/dart-lang/sdk/issues/44622

### Dart2JS

*   Remove `--no-defer-class-types` and `--no-new-deferred-split`.

### Tools

#### Analyzer

*   Remove the `--use-fasta-parser`, `--preview-dart-2`, and
    `--enable-assert-initializers` command line options. These options haven't
    been supported in a while and were no-ops.

*   Report diagnostics regarding the
    [`@internal`](https://pub.dev/documentation/meta/latest/meta/internal-constant.html)
    annotation.

*   Improve diagnostic-reporting regarding the
    [`@doNotStore`](https://pub.dev/documentation/meta/latest/meta/doNotStore-constant.html)
    annotation.

*   Introduce a diagnostic which is reported when a library member named `main`
    is not a function.

*   Introduce a diagnostic which is reported when a `main` function's first
    parameter is not a supertype of `List<String>`.

*   Introduce diagnostics for when an `// ignore` comment contains an error code
    which is not being reported, cannot be ignored, or is already being ignored.

*   Report diagnostics when using
    [`@visibleForTesting`](https://pub.dev/documentation/meta/latest/meta/
    visibleForTesting-constant.html) on top-level variables.

*   Fix false positive reports of "unused element" for top-level setters and
    getters.

*   Fix false positive reports regarding `@deprecated` field formal parameters
    at their declaration.

*   For null safety, introduce a diagnostic which reports when a null-check will
    always fail.

*   Fix false positive reports regarding optional parameters on private
    constructors being unused.

*   Introduce a diagnostic which is reported when a constructor includes
    duplicate field formal parameters.

*   Improve the "unused import" diagnostic when multiple import directives share
    a common prefix.

*   Fix false positive "unused import" diagnostic regarding an import which
    provides an extension method which is used.

*   For null safety, improve the messaging of "use of nullable value"
    diagnostics for eight different contexts.

*   Fix false positive reports regarding `@visibleForTesting` members in a
    "hide" combinator of an import or export directive.

*   Improve the messaging of "invalid override" diagnostics.

*   Introduce a diagnostic which is reported when `Future<T>.catchError` is
    called with an `onError` callback which does not return `FutureOr<T>`.

#### dartfmt

*   Don't duplicate comments on chained if elements.

*   Preserve `?` in initializing formal function-typed parameters.

*   Fix performance issue with constructors that have no initializer list.

#### Linter

Updated the Linter to `0.1.129`, which includes:

*   New lints: `avoid_dynamic_calls`, `cast_nullable_to_non_nullable`,
    `null_check_on_nullable_type_parameter`,
    `tighten_type_of_initializing_formals`, `unnecessary_null_checks`, and
    `avoid_type_to_string`.

*   Fix crash in `prefer_collection_literals` when there is no static parameter
    element.

*   Fix false negatives for `prefer_collection_literals` when a LinkedHashSet or
    LinkedHashMap instantiation is passed as the argument to a function in any
    position other than the first.

*   Fix false negatives for `prefer_collection_literals` when a LinkedHashSet or
    LinkedHashMap instantiation is used in a place with a static type other than
    Set or Map.

*   Update to `package_names` to allow leading underscores.

*   Fix crashes in `unnecessary_null_checks` and
    `missing_whitespace_between_adjacent_strings`.

*   Update to `void_checks` for null safety.

*   Fix range error in `unnecessary_string_escapes`.

*   Fix false positives in `unnecessary_null_types`.

*   Fix to `prefer_constructors_over_static_methods` to respect type
    parameters.

*   Update to `always_require_non_null_named_parameters` to be null safety-aware.

*   Update to `unnecessary_nullable_for_final_variable_declarations` to allow
    dynamic.

*   Update `overridden_fields` to not report on abstract parent fields.

*   Fix to `unrelated_type_equality_checks` for null safety.

*   Improvements to `type_init_formals`to allow types not equal to the field
    type.

*   Updates to `public_member_apis` to check generic type aliases.

*   Fix `close_sinks` to handle `this`-prefixed property accesses.

*   Fix `unawaited_futures` to handle `Future` subtypes.

*   Performance improvements to `always_use_package_imports`,
    `avoid_renaming_method_parameters`, `prefer_relative_imports` and
    `public_member_api_docs`.

#### Pub

*   **Breaking**: The Dart SDK constraint is now **required** in `pubspec.yaml`.
    You must include a section like:

    ```yaml
    environment:
      sdk: '>=2.10.0 <3.0.0'
    ```

    See [#44072][].

    For legacy dependencies without an SDK constraint, pub will now assume a
    default language version of 2.7.

*   The top level `pub` executable has been deprecated. Use `dart pub` instead.
    See [dart tool][].

*   New command `dart pub add` that adds new dependencies to your
    `pubspec.yaml`, and a corresponding `dart pub remove` that removes
    dependencies.

*   New option `dart pub upgrade --major-versions` will update constraints in
    your `pubspec.yaml` to match the the _resolvable_ column reported in `dart
    pub outdated`. This allows users to easily upgrade to latest version for all
    dependencies where this is possible, even if such upgrade requires an update
    to the version constraint in `pubspec.yaml`.

    It is also possible to only upgrade the major version for a subset of your
    dependencies using `dart pub upgrade --major-versions <dependencies...>`.

*   New option `dart pub upgrade --null-safety` will attempt to update constraints
    in your `pubspec.yaml`, such that only null-safety migrated versions of
    dependencies are allowed.

*   New option `dart pub outdated --mode=null-safety` that will analyze your
    dependencies for null-safety.

*   `dart pub get` and `dart pub upgrade` will highlight dependencies that have
    been [discontinued](https://dart.dev/tools/pub/publishing#discontinue) on
    pub.dev.

*   `dart pub publish` will now check your pubspec keys for likely typos.

*   `dart pub upgrade package_foo` fetchs dependencies but ignores the
    `pubspec.lock` for `package_foo`, allowing users to only upgrade a subset of
    dependencies.

*   New command `dart pub login` that logs in to pub.dev.

*   The `--server` option to `dart pub publish` and `dart pub uploader` are
    deprecated. Use `publish_to` in your `pubspec.yaml` or set the
    `$PUB_HOSTED_URL` environment variable.

*   `pub global activate` no longer re-precompiles if the current global
    installation was same version.

*   The Flutter SDK constraint upper bound is now ignored in pubspecs and
    deprecated when publishing. See: [flutter-upper-bound-deprecation][].

[flutter-upper-bound-deprecation]: https://dart.dev/go/flutter-upper-bound-deprecation
[#44072]: https://github.com/dart-lang/sdk/issues/44072
[dart tool]: https://dart.dev/tools/dart-tool

## 2.10.5 - 2021-01-21

This is a patch release that fixes a crash in the Dart VM. (issue [#44563][]).

[#44563]: https://github.com/dart-lang/sdk/issues/44563

## 2.10.4 - 2020-11-12

This is a patch release that fixes a crash in the Dart VM (issues [#43941][],
[flutter/flutter#43620][], and [Dart-Code/Dart-Code#2814][]).

[#43941]: https://github.com/dart-lang/sdk/issues/43941
[flutter/flutter#43620]: https://github.com/flutter/flutter/issues/43620
[Dart-Code/Dart-Code#2814]: https://github.com/Dart-Code/Dart-Code/issues/2814

## 2.10.3 - 2020-10-29

This is a patch release that fixes the following issues:
* breaking changes in Chrome 86 that affect DDC (issues [#43750][] and
  [#43193][]).
* compiler error causing incorrect use of positional parameters when named
  parameters should be used instead (issues [flutter/flutter#65324][] and
  [flutter/flutter#68092][]).
* crashes and/or undefined behavor in AOT compiled code (issues [#43770][] and
  [#43786][]).
* AOT compilation of classes with more than 64 unboxed fields
  (issue [flutter/flutter#67803][]).

[#43750]: https://github.com/dart-lang/sdk/issues/43750
[#43193]: https://github.com/dart-lang/sdk/issues/43193
[flutter/flutter#65324]: https://github.com/flutter/flutter/issues/65324
[flutter/flutter#68092]: https://github.com/flutter/flutter/issues/68092
[#43770]: https://github.com/dart-lang/sdk/issues/43770
[#43786]: https://github.com/dart-lang/sdk/issues/43786
[flutter/flutter#67803]: https://github.com/flutter/flutter/issues/67803

## 2.10.2 - 2020-10-15

This is a patch release that fixes a DDC compiler crash (issue [#43589]).

[#43589]: https://github.com/dart-lang/sdk/issues/43589

## 2.10.1 - 2020-10-06

This is a patch release that fixes the following issues:
* crashes when developing Flutter applications (issue [#43464][]).
* non-deterministic incorrect program behaviour and/or crashes (issue
  [flutter/flutter#66672][]).
* uncaught TypeErrors in DDC (issue [#43661][]).

[#43464]: https://github.com/dart-lang/sdk/issues/43464
[flutter/flutter#66672]: https://github.com/flutter/flutter/issues/66672
[#43661]: https://github.com/dart-lang/sdk/issues/43661

## 2.10.0 - 2020-09-28

### Core libraries

#### `dart:io`

*   Adds `Abort` method to class `HttpClientRequest`, which allows users
    to cancel outgoing HTTP requests and stop following IO operations.
*   A validation check is added to `path` of class `Cookie`. Having characters
    ranging from 0x00 to 0x1f and 0x3b (";") will lead to a `FormatException`.
*   The `HttpClient` and `HttpServer` classes now have a 1 MiB limit for the
    total size of the HTTP headers when parsing a request or response, instead
    of the former 8 KiB limit for each header name and value. This limit cannot
    be configured at this time.

#### `dart:typed_data`

*   Class `BytesBuilder` is moved from `dart:io` to `dart:typed_data`.
    It's temporarily being exported from `dart:io` as well.

### `dart:uri`

*   [#42564]: Solved inconsistency in `Uri.https` and `Uri.http` constructors'
    `queryParams` type.

### Dart VM

*   **Breaking Change** [#42982][]: `dart_api_dl.cc` is renamed to
    `dart_api_dl.c` and changed to a pure C file.
*   Introduces `Dart_FinalizableHandle`s. They do auto-delete, and the weakly
    referred object cannot be accessed through them.

### Dart2JS

*   Adds support for deferred loading of types seperately from classes. This
    enables dart2js to make better optimization choices when deferred loading.
    This work is necessary to address unsoundness in the deferred loading
    algorithm. Currently, fixing this unsoundness would result in code bloat,
    but loading types seperately from classes will allow us to fix the
    unsoundness with only a minimal regression. To explicitly disable
    deferred loading of types, pass `--no-defer-class-types`. See the original
    post on the [unsoundness in the deferred loading algorithm][].
*   Enables a new sound deferred splitting algorithm. To explicitly disable
    the new deferred splitting algorithm, pass `--no-new-deferred-split`.
    See the original post on the
    [unsoundness in the deferred loading algorithm][].


[#42982]: https://github.com/dart-lang/sdk/issues/42982
[unsoundness in the deferred loading algorithm]: https://github.com/dart-lang/sdk/blob/302ad7ab2cd2de936254850550aad128ae76bbb7/CHANGELOG.md#dart2js-3

### Tools

#### dartfmt

* Don't crash when non-ASCII whitespace is trimmed.
* Split all conditional expressions (`?:`) when they are nested.
* Handle `external` and `abstract` fields and variables.

#### Linter

Updated the Linter to `0.1.118`, which includes:

* New lint: `unnecessary_nullable_for_final_variable_declarations`.
* Fixed NPE in `prefer_asserts_in_initializer_lists`.
* Fixed range error in `unnecessary_string_escapes`.
* `unsafe_html` updated to support unique error codes.
* Updates to `diagnostic_describe_all_properties` to check for `Diagnosticable`s (not `DiagnosticableMixin`s).
* New lint: `use_late`.
* Fixed `unnecessary_lambdas` to respect deferred imports.
* Updated `public_member_api_docs` to check mixins.
* Updated `unnecessary_statements` to skip `as` expressions.
* Fixed `prefer_relative_imports` to work with path dependencies.

#### Pub

* `pub run` and `pub global run` accepts a `--(no-)-sound-null-safety` flag,
  that is passed to the VM.
* Fix: Avoid multiple recompilation of binaries in global packages.
* Fix: Avoid exponential behaviour of error reporting from the solver.
* Fix: Refresh binstubs after recompile in global run.

## 2.9.3 - 2020-09-08

This is a patch release that fixes DDC to handle a breaking change in Chrome
(issue [#43193][]).

[#43193]: https://github.com/dart-lang/sdk/issues/43193

## 2.9.2 - 2020-08-26

This is a patch release that fixes transient StackOverflow exceptions when
building Flutter applications (issue [flutter/flutter#63560][]).

[flutter/flutter#63560]: https://github.com/flutter/flutter/issues/63560

## 2.9.1 - 2020-08-12

This is a patch release that fixes unhandled exceptions in some Flutter
applications (issue [flutter/flutter#63038][]).

[flutter/flutter#63038]: https://github.com/flutter/flutter/issues/63038

## 2.9.0 - 2020-08-05

### Language

### Core libraries

#### `dart:async`

*   Adds `Stream.multi` constructor creating streams which can be
    listened to more than once, and where each individual listener
    can be controlled independently.

#### `dart:convert`

*   **Breaking Change** [#41100][]: When encoding a string containing unpaired
    surrogates as UTF-8, the unpaired surrogates will be encoded as replacement
    characters (`U+FFFD`). When decoding UTF-8, encoded surrogates will be
    treated as malformed input. When decoding UTF-8 with `allowMalformed: true`,
    the number of replacement characters emitted for malformed input sequences
    has been changed to match the [WHATWG encoding standard][].

[#41100]: https://github.com/dart-lang/sdk/issues/41100
[WHATWG encoding standard]: https://encoding.spec.whatwg.org/#utf-8-decoder

#### `dart:io`

*   [#42006][]: The signature of `exit` has been changed to return the
    `Never`type instead of `void`. since no code will run after it,
*   Class `OSError` now implements `Exception`. This change means `OSError` will
    now be caught in catch clauses catching `Exception`s.
*   Added `InternetAddress.tryParse`.
*   [Abstract Unix Domain Socket][] is supported on Linux/Android now. Using an
    `InternetAddress` with `address` starting with '@' and type being
    `InternetAddressType.Unix` will create an abstract Unix Domain Socket.
*   On Windows, file APIs can now handle files and directories identified by
    long paths (greater than 260 characters). It complies with all restrictions
    from [Long Path on Windows][]. Note that `Directory.current` does not work
    with long path.

[#42006]: https://github.com/dart-lang/sdk/issues/42006
[Abstract Unix Domain Socket]: http://man7.org/linux/man-pages/man7/unix.7.html
[Long Path on Windows]: https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file#maximum-path-length-limitation

#### `dart:html`

*   **Breaking Change**: `CssClassSet.add()` previously returned `null` if the
    `CssClassSet` corresponded to multiple elements. In order to align with the
    null-safe changes in the `Set` interface, it will now return `false`
    instead. The same applies for `CssClassSet.toggle`.

*   `EventStreamSubscription.cancel` method used to return `null`, but since
    `StreamSubscription.cancel` has changed to be non-nullable, this method
    returns an empty `Future` instead. Due to an optimization on `null`
    `Future`s, this method used to complete synchronously, but now that the
    `Future` is empty instead, it completes asynchronously, therefore
    potentially invalidating code that relied on the synchronous side-effect.
    This change will only affect code using sound null-safety. See issue
    [#41653][] for more details.

*   Methods in `Console` have been updated to better reflect the modern Console
    specification. Particularly of interest are `dir` and `table` which take in
    extra optional arguments.

[#41653]: https://github.com/dart-lang/sdk/issues/41653

#### `dart:mirrors`

*   **Breaking Change** [#42714][]: web compilers (dart2js and DDC) now produce
    a compile-time error if `dart:mirrors` is imported.

    Most projects should not be affected. Since 2.0.0 this library was
    unsupported and produced runtime errors on all its APIs. Since then several
    tools already reject code that use `dart:mirrors` including webdev and
    flutter tools, we expect few projects to run into this problem.

[#42714]: https://github.com/dart-lang/sdk/issues/42714

### Tools

#### dartfmt

* Add `--fix-single-cascade-statements`.
* Correctly handle `var` in `--fix-function-typedefs`.
* Preserve leading indentation in fixed doc comments.
* Split outer nested control flow elements.
* Always place a blank line after script tags.
* Don't add unneeded splits on if elements near comments.
* Indent blocks in initializers of multiple-variable declarations.
* Update the null-aware subscript syntax from `?.[]` to `?[]`.

#### Analyzer

* Static analyses with a severity of "error" can no longer be ignored with
  comments (`// ignore: code` and `// ignore_for_file: code`).

#### Linter

Updated the Linter to `0.1.117`, which includes:

* New lint: `do_not_use_environment`.
* New lint: `exhaustive_cases`.
* New lint: `no_default_cases` (experimental).
* New lint: `sized_box_for_whitespace`.
* New lint: `use_is_even_rather_than_modulo`.
* Updated `directives_ordering` to remove third party package special-casing.
* Updated `prefer_is_empty` to special-case assert initializers and const
  contexts.
* Updated `prefer_mixin` to allow "legacy" SDK abstract class mixins.
* Updated `sized_box_for_whitespace` to address false-positives.
* Updated `type_annotate_public_apis` to allow inferred types in final field
  assignments.
* Updated `unnecessary_lambdas` to check for tear-off assignability.
* Updated `unsafe_html` to use a `SecurityLintCode` (making it un-ignorable) and
  to include `Window.open`, `Element.html` and `DocumentFragment.html` in unsafe
  API checks. Also added checks for attributes and methods on extensions.

### Dart VM

*   **Breaking Change** [#41100][]: When printing a string using the `print`
    function, the default implementation (used when not overridden by the
    embedder or the current zone) will print any unpaired surrogates in the
    string as replacement characters (`U+FFFD`). Similarly, the
    `Dart_StringToUTF8` function in the Dart API will convert unpaired
    surrogates into replacement characters.

### Pub
* `pub run` and `pub global run` accepts a `--enable-experiment` flag enabling
  experiments in the Dart VM (and language).
* Warn when publishing the first null-safe version of a package.
* `pub outdated`:
  * If the current version of a dependency is a prerelease
    version, use prereleases for latest if there is no newer stable.
  * Don't require a `pubspec.lock` file. When the lockfile is missing, the
    **Current** column is empty.
* `pub upgrade`: Show summary count of outdated packages after running.
  It will also only show newer packages if they are not prereleases or
  the package is already a prerelease.
* Publishing Flutter plugins using the old plugin format is no longer allowed.
  Plugins using the old plugin format can still be consumed.
* `pub run`: Fix precompilation with relative `PUB_CACHE` paths
  ([#2486](https://github.com/dart-lang/pub/pull/2486)).
* Preserve Windows line endings in `pubspec.lock` if they are already there
  ([#2489](https://github.com/dart-lang/pub/pull/2489)).
* Better terminal color-detection. Use colors in terminals on Windows.
* Fix git folder names in cache, allowing for ssh-style git
  dependencies.
* Fix: Avoid precompilation of dependencies of global packages.

## 2.8.4 - 2020-06-04

This is a patch release that fixes potential memory leaks in the Dart front-end
(issues [#42111][] and [#42112][]).

[#42111]: https://github.com/dart-lang/sdk/issues/42111
[#42112]: https://github.com/dart-lang/sdk/issues/42112

## 2.8.3 - 2020-05-28

This is a patch release that fixes the following issues:
* crashes in Flutter apps (issue [flutter/flutter#57318][]).
* a regression in stack traces (issue [#41907][]).
* re-canonicalization of constants with unboxed fields (issue
[flutter/flutter#57190][]).

[flutter/flutter#57318]: https://github.com/flutter/flutter/issues/57318
[#41907]: https://github.com/dart-lang/sdk/issues/41907
[flutter/flutter#57190]: https://github.com/flutter/flutter/issues/57190

## 2.8.2 - 2020-05-13

This is a patch release that fixes an AOT compilation bug in global
transformations which manifests as a NoSuchMethod exception
(issue [flutter/flutter#56479][]).

[flutter/flutter#56479]: https://github.com/flutter/flutter/issues/56479

## 2.8.1 - 2020-05-06

Much of the changes in this release are in preparation for non-nullable types,
which will arrive in a future version. In anticipation of that, we have made a
number of small but technically breaking changes to several core library APIs in
order to make them easier to use in a world with non-nullable types. Almost all
existing Dart code will be unaffected by these changes, but if you see
unexpected failures, note the breaking changes listed below.

### Language

There are no new language features in this release. There are only two
minor breaking changes:

*   **Breaking change** [#40675][]: Fixed an implementation bug where local
    variable inference would incorrectly use the promoted type of a type
    variable.

*   **Breaking change** [#41362][]: Dart 2.0.0 made the clauses `implements
    Function`, `extends Function`, or `with Function` have no effect (spec
    section 19.6). We fixed an implementation bug that may be visible on some
    rare scenarios.

[#40675]: https://github.com/dart-lang/sdk/issues/40675
[#41362]: https://github.com/dart-lang/sdk/issues/41362

### Core libraries

#### `dart:async`

*   **Breaking change** [#40676][]: Changed the return type of
    `StreamSubscription.cancel()`  to `Future<void>`. Previously, it was
    declared to return `Future` and was allowed to return `null` at runtime.

*   **Breaking change** [#40681][]: The `runZoned()` function is split into two
    functions: `runZoned()` and `runZonedGuarded()`, where the latter has a
    required `onError` parameter, and the former has none. This prepares the
    functions for null safety where the two functions will differ in the
    nullability of their return types.

*   **Breaking change** [#40683][]: Errors passed to
    `Completer.completeError()`, `Stream.addError()`, `Future.error()`, etc. can
    no longer be `null`. These operations now *synchronously* throw an exception
    if passed a `null` error.

*   Make stack traces non-null [#40130][]. Where methods like
    `completer.completeError()` allow omitting a stack trace, the platform will
    now insert a default stack trace rather than propagate a `null` value.

    Error handling functions need no longer be prepared for `null` stack traces.

[#40676]: https://github.com/dart-lang/sdk/issues/40676
[#40681]: https://github.com/dart-lang/sdk/issues/40681
[#40683]: https://github.com/dart-lang/sdk/issues/40683
[#40130]: https://github.com/dart-lang/sdk/issues/40130

#### `dart:core`

*   **Breaking change** [#40674][]: Three members on `RuneIterator` no longer
    return `null` when accessed before the first call to `moveNext()`. Instead,
    `current` and `rawIndex` return `-1` and `currentAsString` returns an empty
    string.

*   **Breaking change** [#40678][]: The `String.fromEnvironment()` default value
    for `defaultValue` is now an empty string instead of `null`. Likewise, the
    default value for `int.fromEnvironment()`'s `defaultValue` parameter is
    zero. Under null safety, a constructor cannot return `null`, so this
    prepares these APIs for that.

*   The class `CastError` is deprecated, and all implementation specific classes
    implementing `TypeError` or `CastError` now implement both.  In a future
    release, `CastError` will be removed.  See issue [40763][] for details.

*   Adds `StackTrace.empty` constant which is the stack trace used as default
    stack trace when no better alternative is available.

*   The class `TypeError` no longer extends `AssertionError`. This also means
    that it no longer inherits the spurious `message` getter which was added to
    `AssertionError` when the second operand to `assert` was allowed. The value
    of that getter on a `TypeError` was the same string as returned by
    `toString()`, so it is still available.

*   `ArgumentError.checkNotNull()` and the `RangeError` static methods
    `checkValueInInterval()`, `checkValidIndex()` and `checkNotNegative()` all
    return their first argument on success. This makes these functions more
    convenient to use in-line in, for example, `=>` function bodies or
    constructor initialization lists.

[#40674]: https://github.com/dart-lang/sdk/issues/40674
[#40678]: https://github.com/dart-lang/sdk/issues/40678
[40763]: https://github.com/dart-lang/sdk/issues/40763

#### `dart:developer`

*   The constructors for `TimelineTask` now accept an optional `filterKey`
    parameter. If provided, the arguments for all events associated with the
    task will contain an entry named `filterKey`, set to the value of the
    `filterKey` parameter provided in the constructor. This will be used by
    tooling to allow for better filtering of timeline events.

#### `dart:html`

*   **Breaking Change** [#39627][]: Changed the return type of several HTML
    native methods involving futures. In return types that matched
    `Future<List<T>>`, `T was` changed to `dynamic`. These methods would have
    resulted in a runtime error if they were used.

*   **Breaking Change**: `Node.insertAllBefore()` erroneously had a return type
    of `Node`, even though it was not returning anything. This has been
    corrected to `void`.

[#39627]: https://github.com/dart-lang/sdk/issues/39627

#### `dart:io`

*   Class `HttpParser` will no longer throw an exception when a HTTP response
    status code is within [0, 999]. Customized status codes in this range are
    now valid.

*   **Breaking change** [#33501][]: The signature of `HttpHeaders` methods
    `add()` and `set` have been changed to:

    ```dart
    void add(String name, Object value, {bool preserveHeaderCase: false})
    void set(String name, Object value, {bool preserveHeaderCase: false})
    ```

    Setting `preserveHeaderCase` to `true` preserves the case of the `name`
    parameter instead of converting it to lowercase. The `HttpHeader.forEach()`
    method provides the current case of each header.

    This is breaking only for classes extending or implementing `HttpHeaders`
    and having their own `add` or `set` methods without the `bool
    preserveHeaderCase` named parameter.

*   **Breaking change** [#40702][]: The `Socket` class now throws a
    `SocketException` if the socket has been explicitly destroyed or upgraded to
    a secure socket upon setting or getting socket options. Previously, setting
    a socket option would be ignored and getting a socket option would return
    `null`.

*   **Breaking change** [#40483][]: The `Process` class now throws a
    `StateError` if the process is detached (`ProcessStartMode.detached` and
    `ProcessStartMode.detachedWithStdio`) upon accessing the `exitCode` getter.
    It now also throws when not connected to the child process's stdio
    (`ProcessStartMode.detached` and `ProcessStartMode.inheritStdio`) upon
    accessing the `stdin`, `stdout`, and `stderr` getters. Previously, these
    getters would all return `null`.

*   **Breaking change** [#40706][]: The dummy object returned if
    `FileStat.stat()` or `FileStat.statSync()` fail now contains Unix epoch
    timestamps instead of `null` for the `accessed`, `changed`, and `modified`
    getters.

*   **Breaking change** [#40709][]: The `HeaderValue` class now parses more
    strictly in two invalid edge cases. This is the class used to parse the
    semicolon delimited parameters used in the `Accept`, `Authorization`,
    `Content-Type`, and other such HTTP headers.

    The empty parameter value without double quotes (which is not allowed by the
    standards) is now parsed as the empty string rather than `null`. E.g.
    `HeaderValue.parse("v;a=").parameters` now gives `{"a": ""}` rather than
    `{"a": null}`.

    Invalid inputs with unbalanced double quotes are now rejected. E.g.
    `HeaderValue.parse('v;a="b').parameters` will now throw a `HttpException`
    instead of giving `{"a": "b"}`.

*   The `HeaderValue.toString()` method now supports parameters with `null`
    values by omitting the value. `HeaderValue("v", {"a": null, "b":
    "c"}).toString()` now gives `v; a; b=c`. This behavior can be used to
    implement some features in the `Accept` and `Sec-WebSocket-Extensions`
    headers.

    Likewise the empty value and values using characters outside of [RFC 7230
    tokens][] are now correctly implemented by double quoting such values with
    escape sequences. For example:

    ```dart
    HeaderValue("v",
        {"a": "A", "b": "(B)", "c": "", "d": "ø", "e": "\\\""}).toString()
    ```

    Gives: `v;a=A;b="(B)";c="";d="ø";e="\\\""`.

*   [Unix domain sockets][] are now supported on Linux, Android and MacOS, which
    can be used by passing a `InternetAddress` of `InternetAddressType.Unix`
    into the `connect()`, `startConnect()` and `bind()` methods. The `port`
    argument in those methods will be ignored. The `port` getter always returns
    0 for Unix domain sockets.

*   Class `InternetAddressType` gains one more option `Unix`, which represents a
    Unix domain address.

*   Class `InternetAddress`:

    *   `InternetAddress` constructor gains an optional `type` parameter. To
        create a Unix domain address, `type` is set to
        `InternetAddressType.Unix` and `address` is a file path.

    *   `InternetAddress` gains a new constructor `fromRawAddress()` that takes
        an address in byte format for Internet addresses or raw file path for
        Unix domain addresses.

*   **Breaking change** [#40681][]: The static methods `runZoned()` and
    `runWithHttpOverrides()` on `HttpOverrides` no longer accept
    `zoneSpecification` and `onError` parameters. Use the `runZoned()` or
    `runZonedGuarded()` functions from `dart:async` directly if needing to
    specify those.

*   Class `HttpClient` and `HttpServer`, when receiving `HttpRequest` or
    `HttpClientResponse`, will now put a 8K size limit on its header fields and
    values.

[#33501]: https://github.com/dart-lang/sdk/issues/33501
[#40702]: https://github.com/dart-lang/sdk/issues/40702
[#40483]: https://github.com/dart-lang/sdk/issues/40483
[#40706]: https://github.com/dart-lang/sdk/issues/40706
[#40709]: https://github.com/dart-lang/sdk/issues/40709
[RFC 7230 tokens]: https://tools.ietf.org/html/rfc7230#section-3.2.6
[Unix domain sockets]: https://en.wikipedia.org/wiki/Unix_domain_socket

#### `dart:mirrors`

*   Added `MirrorSystem.neverType`.

### Dart VM

*   Added `Dart_TypeDynamic`, `Dart_TypeVoid` and `Dart_TypeNever`. Type
    `dynamic` can no longer by reached using `Dart_GetType(dart:core, dynamic)`.

*   Added the following methods to the VM embedding API:

    * `Dart_GetNonNullableType()`
    * `Dart_GetNullableType()`
    * `Dart_TypeToNonNullable()`
    * `Dart_TypeToNullable()`
    * `Dart_IsLegacyType()`
    * `Dart_IsNonNullableType()`
    * `Dart_IsNullableType()`

### Foreign Function Interface (`dart:ffi`)

*   **Breaking Change**: Changed `Pointer.asFunction()` and
    `DynamicLibrary.lookupFunction()` to extension methods. Invoking them
    dynamically previously already threw an exception, so the runtime behavior
    stays the same. However, the extension methods are only visible if
    `dart:ffi` is imported directly. This breaks code where `dart:ffi` is not
    directly imported. To fix, add:

    ```dart
    import 'dart:ffi';
    ```

### Tools

#### Dart Dev Compiler (DDC)

We fixed several inconsistencies between DDC and Dart2JS so that users less
frequently encounter code that is accepted by one compiler but then fails in the
other.

*   **Breaking Change**: Deleted the legacy (analyzer based) version of DDC. For
    additional details see the [announcement][ddc].

    *   The `--kernel` option is now ignored and defaults to true. There is no
        longer any way to invoke the legacy (analyzer based) version of DDC.

    *   Command line arguments that were only used for the legacy DDC have been
        removed.

    *   The pre-compiled `dart_sdk.js` artifacts generated by legacy DDC have
        been deleted from `dart-sdk/lib/dev_compiler` in favor of the versions
        located at `dart-sdk/lib/dev_compiler/kernel`.

*   **Breaking Change**: Functions passed to JavaScript using the recommended
    `package:js` interop specification must now be wrapped with a call to
    `allowInterop`. This behavior was always enforced by Dart2JS, but was not
    enforced consistently by DDC. It is now enforced by both.

*   **Breaking Change**: Constructors in `@JS()` classes must be marked with
    `external`. Previously the `external` could be omitted in some cases with
    DDC but doing so would cause incorrect behavior with Dart2JS.

*   JS interop classes with an index operator are now static errors.

*   All remaining support from the `dart:mirrors` library has been removed. Use
    of this library on the web has been unsupported and prevented by the Dart
    build systems since Dart v2.0.0. All known exception cases have been cleaned
    up. This change makes DDC and Dart2JS now behave consistently.

    The library can still be imported on web apps, but all APIs throw. In a
    future breaking change release, imports to this library will likely become a
    compile-time error.

[ddc]: https://github.com/dart-lang/sdk/issues/38994

#### Dart2JS

A new representation of runtime types is enabled by default.

This change is part of a long term goal of making runtime checks cheaper and
more flexible for upcoming changes in the language. The new representation
disentangles how types and classes are represented and makes types first-class
to the compiler. This makes it possible to do certain kind of optimizations on
type checks that were not possible before and will enable us to model
non-nullable types in the near future.

This change should not affect the semantics of your application, but it has some
relatively small visible effects that we want to highlight:

*   Types are now canonicalized, this fixes a long standing bug that Types could
    not be used in switch cases (issue [17207][]).

*   Code-size changes may be visible, but the difference is small overall. It is
    more visible on smaller apps because the new implementation includes more
    helper methods. On large apps we have even seen an overall code-size
    reduction.

*   Certain checks are a lot faster.  This is less noticeable if you are
    compiling apps with `-O3` where checks are omitted altogether. Even with
    `-O3`, the performance of some `is` checks used by your app may improve.

*   When using `-O3` and `-O4` incorrect type annotations could surface as
    errors. The old type representation was accidentally lenient on some invalid
    type annotations. We have only encountered this issue on programs that were
    not tested properly at the js-interop program boundary.

*   `Type.toString()` has a small change that is rarely visible. For a long
    time, Dart2JS has had support to erase unused type variables. Today, when
    Dart2JS is given `--lax-runtime-type-to-string` (currently included in
    `-O2`, `-O3`, and `-O4`) and it decides to erase the type variable of a
    class `Foo<T>`, then it compiles expressions like
    `foo.runtimeType.toString()` to print `Foo`. With the new representation,
    this will show `Foo<erased>` instead. This change may be visible in error
    messages produced by type checks involving erased types.

Because types and classes are represented separately, we will likely reevaluate
restrictions of deferred libraries in the near future. For example, we could
support referring to deferred types because types can be downloaded while
classes are not.

In the unlikely case you run into any issues, please file a bug so we can
investigate. You can temporarily force the old type representation by passing
`--use-old-rti` to Dart2JS if necessary, but our goal is to delete the old type
representation soon.

In addition, we fixed some inconsistencies between Dart2JS and DDC:

*   JS interop classes with an index operator are now static errors instead of
    causing invalid code in Dart2JS.

*   **Breaking Change**: The subtyping rule for generic functions is now more
    forgiving. Corresponding type parameter bounds now only need to be mutual
    subtypes rather than structurally equal up to renaming of bound type
    variables and equating all top types.

*   **Breaking Change**: Types are now normalized. See [normalization][] for the
    full specification. Types will now be printed in their normal form, and
    mutual subtypes with the same normal form will now be considered equal.

*   **Breaking Change**: Constructors in `@JS()` classes must be marked with
    `external`. Previously, the external could be omitted for unused
    constructors. Omitting `external` for a constructor which is used would
    cause incorrect behavior at runtime, now omitting it on any constructor is a
    static error.

[17207]: https://github.com/dart-lang/sdk/issues/17207

[normalization]: https://github.com/dart-lang/language/blob/master/resources/type-system/normalization.md


Other dart2js changes:

* **Breaking Change**: The `--package-root` flag, which was hidden and disabled
  in Dart 2.0.0, has been completely removed. Passing this flag will now cause
  `dart2js` to fail.

#### Linter

Updated the Linter to `0.1.114`, which includes:

* Fixed `avoid_shadowing_type_parameters` to support extensions and mixins.
* Updated `non_constant_identifier_names` to allow named constructors made up of only underscores (`_`).
* Updated `avoid_unused_constructor_parameters` to ignore unused params named in all underscores (`_`).

#### Analyzer

*   Removed support for the deprecated analysis options file name
    `.analysis_options`.

#### Pub

*   Added `pub outdated` command which lists outdated package dependencies, and
    gives advice on how to upgrade.

*   `pub get` and `pub upgrade` now fetch version information about hosted
    dependencies in parallel, improving the time package resolution performance.

*   `pub get` and `pub upgrade` no longer precompile executables from
    dependencies by default. Instead they are precompiled on first `pub run`.
    Use `pub get --precompile` to get the previous behavior.

*   Fixed missing retries of DNS failures during `pub get`.

*   If code contains imports for packages not listed in the package's
    `pubspec.yaml` then `pub publish` will reject the package.

*   `pub publish` no longer requires the presence of a `homepage` field, if the
    `repository` field is provided.

*   `pub publish` warns if non-pre-release packages depends on pre-release
    packages or pre-release Dart SDKs.

*   Relative paths in `pubspec.lock` now use `/` also on Windows to make the
    file sharable between machines.

*   Fixed language version in [`.dart_tool/package_config.json`][package config]
    for packages without an explicit SDK constraint. Pub now writes an empty
    language version where before the language version of the current SDK would
    be used.

*   `%LOCALAPPDATA%` is now preferred over `%APPDATA%` when creating a pub cache
    directory on Windows. `%LOCALAPPDATA%` is not copied when users roam between
    devices.

*   `pub publish` warns if LICENSE and README.md files are not called those
    exact names.

*   `pub repair cache` downloads hosted packages in parallel.

[package config]: https://github.com/dart-lang/language/blob/master/accepted/future-releases/language-versioning/package-config-file-v2.md

## 2.7.2 - 2020-03-23

This is a patch release that addresses a vulnerability in `dart:html`
[NodeValidator][] related to DOM clobbering of `previousSibling`. See the
[vulnerability advisory][CVE-2020-8923] for more details. Thanks to **Vincenzo
di Cicco** for finding and reporting this issue.

This release also improves compatibility with ARMv8 processors (issue [40001][])
and dart:io stability (issue [40589][]).

[NodeValidator]: https://api.dart.dev/stable/dart-html/NodeValidator-class.html
[CVE-2020-8923]: https://github.com/dart-lang/sdk/security/advisories/GHSA-hfq3-v9pv-p627
[40001]: https://github.com/dart-lang/sdk/issues/40001
[40589]: https://github.com/dart-lang/sdk/issues/40589

## 2.7.1 - 2020-01-23

This is a patch release that improves dart2js compile-time (issue [40217][]).

[40217]: https://github.com/dart-lang/sdk/issues/40217

**Breaking Change**:
The Dart SDK for macOS is now only available for x64 (issue [39810][]).

[39810]: https://github.com/dart-lang/sdk/issues/39810

## 2.7.0 - 2019-12-11

**Extension methods** -- which we shipped in preview in 2.6.0 -- are no longer
in preview, and are now officially supported as of 2.7.0. Learn more about them
here:

https://medium.com/dartlang/extension-methods-2d466cd8b308

### Language

* **Breaking Change**: [Static extension members][] are accessible when
imported with a prefix (issue [671][]). In the extension method **preview**
launch, importing a library with a prefix hid all extension members in
addition to hiding the extension name, thereby making them inaccessible
in the importing library except via the explicit override syntax. Based
on user feedback, we have changed this to make extensions methods accessible
even when imported with a prefix.

    ```dart
      // "thing.dart"
      class Thing {
      }
      extension Show on Thing {
        void show() {
          print("This is a thing");
        }
     }
     // "client.dart"
     import "thing.dart" as p;
     void test() {
       p.Thing().show(); // Previously an error, now resolves to Show.show
     }
    ```

[Static extension members]: https://github.com/dart-lang/language/blob/master/accepted/2.6/static-extension-members/feature-specification.md
[671]: https://github.com/dart-lang/language/issues/671

### Core libraries

#### `dart:io`

* **Breaking change**: Added `IOOverrides.serverSocketBind` to aid in writing
  tests that wish to mock `ServerSocket.bind`.

#### `dart:typed_data`

* Added new constructors, `.sublistView(TypedData, [start, end])` to all
  `TypedData` classes. The constructor makes it easier, and less error-prone,
  to create a view of (a slice of) another `TypedData` object.

### Dart VM

* New fields added to existing instances by a reload will now be initialized
  lazily, as if the field was a late field. This makes the initialization order
  program-defined, whereas previously it was undefined.

### Tools

#### Linter

The Linter was updated to `0.1.104`, which includes:

* updated `unnecessary_overrides` to allow overrides when annotations (besides `@override` are specified)
* updated `file_names` to allow names w/ leading `_`'s (and improved performance)
* new lint: `unnecessary_final`

#### Pub

* `pub get` generates [`.dart_tool/package_config.json`](https://github.com/dart-lang/language/blob/62c036cc41b10fb543102d2f73ee132d1e2b2a0e/accepted/future-releases/language-versioning/package-config-file-v2.md)
  in addition to `.packages` to support language versioning.

* `pub publish` now warns about the old flutter plugin registration format.

* `pub publish` now warns about the `author` field in pubspec.yaml being.
  obsolete.

* Show a proper error message when `git` is not installed.

## 2.6.1 - 2019-11-11

This is a patch release that reduces dart2js memory usage (issue [27883][]),
improves stability on arm64 (issue [39090][]) and updates the Dart FFI
documentation.

[27883]: https://github.com/dart-lang/sdk/issues/27883
[39090]: https://github.com/dart-lang/sdk/issues/39090

## 2.6.0 - 2019-11-05

### Language

*   **[IN PREVIEW]** [Static extension members][]: A new language feature
    allowing specially declared static functions to be invoked like instance
    members on expressions of appropriate static types is available in preview.

    Static extension members are declared using a new `extension` declaration.
    Example:

    ```dart
    extension MyFancyList<T> on List<T> {
      /// Whether this list has an even length.
      bool get isLengthEven => this.length.isEven;

      /// Whether this list has an odd length.
      bool get isLengthOdd => !isLengthEven;

      /// List of values computed for each pairs of adjacent elements.
      ///
      /// The result always has one element less than this list,
      /// if this list has any elements.
      List<R> combinePairs<R>(R Function(T, T) combine) =>
          [for (int i = 1; i < this.length; i++)
              combine(this[i - 1], this[i])];
    }
    ```

    Extension declarations cannot declare instance fields or constructors.
    Extension members can be invoked explicitly,
    `MyFancyList(intList).isLengthEven)`, or implicitly, `intList.isLengthEven`,
    where the latter is recognized by `intList` matching the `List<T>` "on" type
    of the declaration. An extension member cannot be called implicitly on an
    expression whose static type has a member with the same base-name. In that
    case, the interface member takes precedence. If multiple extension members
    apply to the same implicit invocation, the most specific one is used, if
    there is one such.

    Extensions can be declared on any type, not just interface types.

    ```dart
    extension IntCounter on int {
      /// The numbers from this number to, but not including, [end].
      Iterable<int> to(int end) sync* {
        int step = end < this ? -1 : 1;
        for (int i = this; i != end; i += step) yield i;
      }
    }

    extension CurryFunction<R, S, T> on R Function(S, T) {
      /// Curry a binary function with its first argument.
      R Function(T) curry(S first) => (T second) => this(first, second);
    }
    ```

    [Static extension members]: https://github.com/dart-lang/language/blob/master/accepted/2.6/static-extension-members/feature-specification.md

*   **Breaking change** [#37985](https://github.com/dart-lang/sdk/issues/37985):
    Inference is changed when using `Null` values in a `FutureOr` context.
    Namely, constraints of the forms similar to `Null` <: `FutureOr<T>` now
    yield `Null` as the solution for `T`.  For example, the following code will
    now print "Null", and it was printing "dynamic" before (note that the
    anonymous closure `() {}` in the example has `Null` as its return type):

    ```dart
    import 'dart:async';

    void foo<T>(FutureOr<T> Function() f) { print(T); }

    main() { foo(() {}); }
    ```


### Core libraries

* Default values of parameters of abstract methods are no longer available
  via `dart:mirrors`.

#### `dart:developer`

* Added optional `parent` parameter to `TimelineTask` constructor to allow for
  linking of asynchronous timeline events in the DevTools timeline view.

#### `dart:io`

* Added `enableTimelineLogging` property to `HttpClient` which, when enabled,
  will post HTTP connection and request information to the developer timeline
  for all `HttpClient` instances.

### Dart VM

* Added a new tool for AOT compiling Dart programs to native, self-contained
executables. See https://dart.dev/tools/dart2native for additional details.

### Foreign Function Interface (`dart:ffi`)

*   **Breaking change**: The API now makes use of static extension members.
    Static extension members enable the `dart:ffi` API to be more precise with
    types, and provide convenient access to memory through extension getters and
    setters. The extension members on `Pointer` provide `.value` and `.value =`
    for accessing the value in native memory and `[]` and `[]=` for indexed access.
    The method `asExternalTypedData` has been replaced with `asTypedList` extension
    methods. And finally, `Structs` do no longer have a type argument and are
    accessed using the extension member `.ref` on `Pointer`.
    These changes makes the code using `dart:ffi` much more concise.
*   **Breaking change**: The memory management has been removed (`Pointer.allocate`
    and `Pointer.free`). Instead, memory management is available in
    [package:ffi](https://pub.dev/packages/ffi).
*   **Breaking change**: `Pointer.offsetBy` was removed, use `cast` and `elementAt`
    instead.
*   Faster memory load and stores.
*   The dartanalyzer (commandline and IDEs) now reports `dart:ffi` static errors.
*   Callbacks are now supported in AOT (ahead-of-time) compiled code.

### Dart for the Web

#### Dart Dev Compiler (DDC)

* Kernel DDC will no longer accept non-dill files as summary inputs.
* Removed support for the deprecated web extension.

### Tools

#### Linter

The Linter was updated to `0.1.101`, which includes:

* fixed `diagnostic_describe_all_properties` to flag properties in `Diagnosticable`s with no debug methods defined
* fixed `noSuchMethod` exception in `camel_case_extensions` when analyzing unnamed extensions
* fixed `avoid_print` to catch tear-off usage
* new lint: `avoid_web_libraries_in_flutter` (experimental)
* (internal) prepare `unnecessary_lambdas` for coming `MethodInvocation` vs. `FunctionExpressionInvocation` changes

## 2.5.2 - 2019-10-08

This is a patch release with properly signed binaries required for macOS
Catalina (Issue [38765][]).

[38765]: https://github.com/dart-lang/sdk/issues/38765

## 2.5.1 - 2019-09-27

This is a patch release that prevents type inference failures in the analyzer
(Issue [38365][]).

[38365]: https://github.com/dart-lang/sdk/issues/38365

## 2.5.0 - 2019-09-10

### Language

The set of operations allowed in constant expressions has been expanded as
described in
the [constant update proposal](https://github.com/dart-lang/language/issues/61).
The control flow and spread collection features shipped in Dart 2.3 are now also
supported in constants
as
[described in the specification here](https://github.com/dart-lang/language/blob/master/accepted/2.3/unified-collections/feature-specification.md#constant-semantics).

Specifically, it is now valid to use the following operations in constant
expressions under the appropriate conditions:
  - Casts (`e as T`) and type tests (`e is T`).
  - Comparisons to `null`, even for types which override the `==` operator.
  - The `&`, `|`, and `^` binary operators on booleans.
  - The spread operators (`...` and `...?`).
  - An `if` element in a collection literal.

```dart
// Example: these are now valid constants.
const Object i = 3;
const list = [i as int];
const set = {if (list is List<int>) ...list};
const map = {if (i is int) i : "int"};
```

In addition, the semantics of constant evaluation has been changed as follows:
  - The `&&` operator only evaluates its second operand if the first evaluates
  to true.
  - The `||` operator only evaluates its second operand if the first evaluates
  to false.
  - The `??` operator only evaluates its second operand if the first evaluates
  to null.
  - The conditional operator (`e ? e1 : e2`) only evaluates one of the two
    branches, depending on the value of the first operand.

```dart
// Example: x is now a valid constant definition.
const String s = null;
const int x = (s == null) ? 0 : s.length;
```

### Core libraries

* **Breaking change** [#36900](https://github.com/dart-lang/sdk/issues/36900):
  The following methods and
  properties across various core libraries, which used to declare a return type
  of `List<int>`, were updated to declare a return type of `Uint8List`:

  * `BytesBuilder.takeBytes()`
  * `BytesBuilder.toBytes()`
  * `Datagram.data`
  * `File.readAsBytes()` (`Future<Uint8List>`)
  * `File.readAsBytesSync()`
  * `InternetAddress.rawAddress`
  * `RandomAccessFile.read()` (`Future<Uint8List>`)
  * `RandomAccessFile.readSync()`
  * `RawSocket.read()`
  * `Utf8Codec.encode()` (and `Utf8Encoder.convert()`)

  In addition, the following classes were updated to implement
  `Stream<Uint8List>` rather than `Stream<List<int>>`:

  * `HttpRequest`
  * `Socket`

  **Possible errors and how to fix them**

    * > The argument type 'Utf8Decoder' can't be assigned to the parameter type 'StreamTransformer<Uint8List, dynamic>'

      > type 'Utf8Decoder' is not a subtype of type 'StreamTransformer' of 'streamTransformer'"

      You can fix these call sites by updating your code to use
      `StreamTransformer.bind()` instead of `Stream.transform()`, like so:

      *Before:* `stream.transform(utf8.decoder)`
      *After:* `utf8.decoder.bind(stream)`

    * > The argument type 'IOSink' can't be assigned to the parameter type 'StreamConsumer<Uint8List>'

      > type '_IOSinkImpl' is not a subtype of type 'StreamConsumer<Uint8List>' of 'streamConsumer'

      You can fix these call sites by casting your stream instance to a `Stream<List<int>>` before calling `.pipe()` on the stream, like so:

      *Before:* `stream.pipe(consumer)`
      *After:* `stream.cast<List<int>>().pipe(consumer)`

  Finally, the following typed lists were updated to have their `sublist()`
  methods declare a return type that is the same as the source list:

  * `Int8List.sublist()` → `Int8List`
  * `Int16List.sublist()` → `Int16List`
  * `Int32List.sublist()` → `Int32List`
  * `Int64List.sublist()` → `Int64List`
  * `Int32x4List.sublist()` → `Int32x4List`
  * `Float32List.sublist()` → `Float32List`
  * `Float64List.sublist()` → `Float64List`
  * `Float32x4List.sublist()` → `Float32x4List`
  * `Float64x2List.sublist()` → `Float64x2List`
  * `Uint8List.sublist()` → `Uint8List`
  * `Uint8ClampedList.sublist()` → `Uint8ClampedList`
  * `Uint16List.sublist()` → `Uint16List`
  * `Uint32List.sublist()` → `Uint32List`
  * `Uint64List.sublist()` → `Uint64List`

#### `dart:async`

* Add `value` and `error` constructors on `Stream`
  to allow easily creating single-value or single-error streams.

#### `dart:core`

* Update `Uri` class to support [RFC6874](https://tools.ietf.org/html/rfc6874):
  "%25" or "%" can be appended to the end of a valid IPv6 representing a Zone
  Identifier. A valid zone ID consists of unreversed character or Percent
  encoded octet, which was defined in RFC3986.
  IPv6addrz = IPv6address "%25" ZoneID

  [29456]: https://github.com/dart-lang/sdk/issues/29456

#### `dart:io`

* **Breaking change** [#37192](https://github.com/dart-lang/sdk/issues/37192):
  The `Cookie` class's constructor's `name` and `value`
  optional positional parameters are now mandatory. The
  signature changes from:

      Cookie([String name, String value])

  to

      Cookie(String name, String value)

  However, it has not been possible to set `name` and `value` to null since Dart
  1.3.0 (2014) where a bug made it impossible. Any code not using both
  parameters or setting any to null would necessarily get a noSuchMethod
  exception at runtime. This change catches such erroneous uses at compile time.
  Since code could not previously correctly omit the parameters, this is not
  really a breaking change.

* **Breaking change** [#37192](https://github.com/dart-lang/sdk/issues/37192):
  The `Cookie` class's `name` and `value` setters now
  validates that the strings are made from the allowed character set and are not
  null. The constructor already made these checks and this
  fixes the loophole where the setters didn't also validate.

### Dart VM

### Tools

#### Pub

 * Clean-up invalid git repositories in cache when fetching from git.
 * **Breaking change**  [#36765](https://github.com/dart-lang/sdk/issues/36765):
   Packages published to [pub.dev](https://pub.dev) can no longer contain git
   dependencies. These packages will be rejected by the server.

#### Linter

The Linter was updated to `0.1.96`, which includes:

* fixed false positives in `unnecessary_parens`
* various changes to migrate to preferred analyzer APIs
* rule test fixes

#### Dartdoc

Dartdoc was updated to `0.28.4`; this version includes several fixes and is based
on a newer version of the analyzer package.

## 2.4.1 - 2019-08-07

This is a patch release that fixes a performance regression in JIT mode, as
well as a potential crash of our AOT compiler.

### Dart VM

* Fixed a performance regression where usage of `Int32List` could trigger
  repeated deoptimizations in JIT mode (Issue [37551][]).

* Fixed a bug where usage of a static getter with name `length` could cause a
  crash in our AOT compiler (Issue [35121][]).

[37551]: https://github.com/dart-lang/sdk/issues/37551
[35121]: https://github.com/dart-lang/sdk/issues/35121

### Dart Dev Compiler (DDC)

Callbacks passed to JS and wrapped with `allowInterop` or
`allowInteropCaptureThis` are now strict about argument counts and argument
types. This may mean that tests which were previously passing and relying on
loose argument checking (too many or too few arguments, or arguments with too
specific types like `List<Something>` instead of `List<dynamic>`) may start
failing. This changes makes DDC behave more like dart2js with the default flags.

## 2.4.0 - 2019-06-27

### Core libraries

#### `dart:isolate`

* `TransferableTypedData` class was added to facilitate faster cross-isolate
communication of `Uint8List` data.

* **Breaking change**: `Isolate.resolvePackageUri` will always throw an
  `UnsupportedError` when compiled with dart2js or DDC. This was the only
  remaining API in `dart:isolate` that didn't automatically throw since we
  dropped support for this library in [Dart 2.0.0][1]. Note that the API already
  throws in dart2js if the API is used directly without manually setting up a
  `defaultPackagesBase` hook.

[1]: https://github.com/dart-lang/sdk/blob/master/CHANGELOG.md#200---2018-08-07


#### `dart:developer`
* Exposed `result`, `errorCode` and `errorDetail` getters in
  `ServiceExtensionResponse` to allow for better debugging of VM service
  extension RPC results.

#### `dart:io`

* Fixed `Cookie` class interoperability with certain websites by allowing the
  cookie values to be the empty string (Issue [35804][]) and not stripping
  double quotes from the value (Issue [33327][]) in accordance with RFC 6265.

  [33327]: https://github.com/dart-lang/sdk/issues/33327
  [35804]: https://github.com/dart-lang/sdk/issues/35804

* [#36971](https://github.com/dart-lang/sdk/issues/36971):
  The `HttpClientResponse` interface has been extended with the addition of a
  new `compressionState` getter, which specifies whether the body of a
  response was compressed when it was received and whether it has been
  automatically uncompressed via `HttpClient.autoUncompress`.

  As part of this change, a corresponding new enum was added to `dart:io`:
  `HttpClientResponseCompressionState`.

  This is a **breaking change** for those implementing the `HttpClientResponse`
  interface as subclasses will need to implement the new getter.

#### `dart:async`

* **Breaking change** [#36382](https://github.com/dart-lang/sdk/issues/36382):
  The `await for` allowed `null` as a stream due to a bug
  in `StreamIterator` class. This bug has now been fixed.

#### `dart:core`

* [#36171](https://github.com/dart-lang/sdk/issues/36171):
  The `RegExp` interface has been extended with two new
  constructor named parameters:

  * `unicode:` (`bool`, default: `false`), for Unicode patterns
  * `dotAll:` (`bool`, default: `false`), to change the matching behavior of
    '.' to also match line terminating characters.

  Appropriate properties for these named parameters have also been added so
  their use can be detected after construction.

  In addition, `RegExp` methods that originally returned `Match` objects
  now return a more specific subtype, `RegExpMatch`, which adds two features:

  * `Iterable<String> groupNames`, a property that contains the names of all
    named capture groups
  * `String namedGroup(String name)`, a method that retrieves the match for
    the given named capture group

  This is a **breaking change** for implementers of the `RegExp` interface.
  Subclasses will need to add the new properties and may have to update the
  return types on overridden methods.

### Language

*   **Breaking change** [#35097](https://github.com/dart-lang/sdk/issues/35097):
    Covariance of type variables used in super-interfaces
    is now enforced. For example, the following code was
    previously accepted and will now be rejected:

```dart
class A<X> {};
class B<X> extends A<void Function(X)> {};
```

* The identifier `async` can now be used in asynchronous and generator
  functions.

### Dart for the Web

#### Dart Dev Compiler (DDC)

* Improve `NoSuchMethod` errors for failing dynamic calls. Now they include
  specific information about the nature of the error such as:
  * Attempting to call a null value.
  * Calling an object instance with a null `call()` method.
  * Passing too few or too many arguments.
  * Passing incorrect named arguments.
  * Passing too few or too many type arguments.
  * Passing type arguments to a non-generic method.

### Tools

#### Linter

The Linter was updated to `0.1.91`, which includes the following changes:

* Fixed missed cases in `prefer_const_constructors`
* Fixed `prefer_initializing_formals` to no longer suggest API breaking changes
* Updated `omit_local_variable_types` to allow explicit `dynamic`s
* Fixed null-reference in `unrelated_type_equality_checks`
* New lint: `unsafe_html`
* Broadened `prefer_null_aware_operators` to work beyond local variables.
* Added `prefer_if_null_operators`.
* Fixed `prefer_contains` false positives.
* Fixed `unnecessary_parenthesis` false positives.
* Fixed `prefer_asserts_in_initializer_lists` false positives
* Fixed `curly_braces_in_flow_control_structures` to handle more cases
* New lint: `prefer_double_quotes`
* New lint: `sort_child_properties_last`
* Fixed `type_annotate_public_apis` false positive for `static const` initializers

#### Pub

* `pub publish` will no longer warn about missing dependencies for import
   statements in `example/`.
* OAuth2 authentication will explicitly ask for the `openid` scope.

## 2.3.2 - 2019-06-11

This is a patch version release with a security improvement.

### Security vulnerability

*  **Security improvement:** On Linux and Android, starting a process with
   `Process.run`, `Process.runSync`, or `Process.start` would first search the
   current directory before searching `PATH` (Issue [37101][]). This behavior
   effectively put the current working directory in the front of `PATH`, even if
   it wasn't in the `PATH`. This release changes that behavior to only searching
   the directories in the `PATH` environment variable. Operating systems other
   than Linux and Android didn't have this behavior and aren't affected by this
   vulnerability.

   This vulnerability could result in execution of untrusted code if a command
   without a slash in its name was run inside an untrusted directory containing
   an executable file with that name:

   ```dart
   Process.run("ls", workingDirectory: "/untrusted/directory")
   ```

   This would attempt to run `/untrusted/directory/ls` if it existed, even
   though it is not in the `PATH`. It was always safe to instead use an absolute
   path or a path containing a slash.

   This vulnerability was introduced in Dart 2.0.0.

[37101]: https://github.com/dart-lang/sdk/issues/37101

## 2.3.1 - 2019-05-21

This is a patch version release with bug fixes.

### Tools

#### dart2js

* Fixed a bug that caused the compiler to crash when it compiled UI-as-code
  features within fields (Issue [36864][]).

[36864]: https://github.com/dart-lang/sdk/issues/36864

## 2.3.0 - 2019-05-08

The focus in this release is on the new "UI-as-code" language features which
make collections more expressive and declarative.

### Language

Flutter is growing rapidly, which means many Dart users are building UI in code
out of big deeply-nested expressions. Our goal with 2.3.0 was to [make that kind
of code easier to write and maintain][ui-as-code]. Collection literals are a
large component, so we focused on three features to make collections more
powerful. We'll use list literals in the examples below, but these features also
work in map and set literals.

[ui-as-code]: https://medium.com/dartlang/making-dart-a-better-language-for-ui-f1ccaf9f546c

#### Spread

Placing `...` before an expression inside a collection literal unpacks the
result of the expression and inserts its elements directly inside the new
collection. Where before you had to write something like this:

```dart
CupertinoPageScaffold(
  child: ListView(children: [
    Tab2Header()
  ]..addAll(buildTab2Conversation())
    ..add(buildFooter())),
);
```

Now you can write this:

```dart
CupertinoPageScaffold(
  child: ListView(children: [
    Tab2Header(),
    ...buildTab2Conversation(),
    buildFooter()
  ]),
);
```

If you know the expression might evaluate to null and you want to treat that as
equivalent to zero elements, you can use the null-aware spread `...?`.

#### Collection if

Sometimes you might want to include one or more elements in a collection only
under certain conditions. If you're lucky, you can use a `?:` operator to
selectively swap out a single element, but if you want to exchange more than one
or omit elements, you are forced to write imperative code like this:

```dart
Widget build(BuildContext context) {
  var children = [
    IconButton(icon: Icon(Icons.menu)),
    Expanded(child: title)
  ];

  if (isAndroid) {
    children.add(IconButton(icon: Icon(Icons.search)));
  }

  return Row(children: children);
}
```

We now allow `if` inside collection literals to conditionally omit or (with
`else`) swap out an element:

```dart
Widget build(BuildContext context) {
  return Row(
    children: [
      IconButton(icon: Icon(Icons.menu)),
      Expanded(child: title),
      if (isAndroid)
        IconButton(icon: Icon(Icons.search)),
    ],
  );
}
```

Unlike the existing `?:` operator, a collection `if` can be composed with
spreads to conditionally include or omit multiple items:

```dart
Widget build(BuildContext context) {
  return Row(
    children: [
      IconButton(icon: Icon(Icons.menu)),
      if (isAndroid) ...[
        Expanded(child: title),
        IconButton(icon: Icon(Icons.search)),
      ]
    ],
  );
}
```

#### Collection for

In many cases, the higher-order methods on Iterable give you a declarative way
to modify a collection in the context of a single expression. But some
operations, especially involving both transforming and filtering, can be
cumbersome to express in a functional style.

To solve this problem, you can use `for` inside a collection literal. Each
iteration of the loop produces an element which is then inserted in the
resulting collection. Consider the following code:

```dart
var command = [
  engineDartPath,
  frontendServer,
  ...fileSystemRoots.map((root) => "--filesystem-root=$root"),
  ...entryPoints
      .where((entryPoint) => fileExists("lib/$entryPoint.json"))
      .map((entryPoint) => "lib/$entryPoint"),
  mainPath
];
```

With a collection `for`, the code becomes simpler:

```dart
var command = [
  engineDartPath,
  frontendServer,
  for (var root in fileSystemRoots) "--filesystem-root=$root",
  for (var entryPoint in entryPoints)
    if (fileExists("lib/$entryPoint.json")) "lib/$entryPoint",
  mainPath
];
```

As you can see, all three of these features can be freely composed. For full
details of the changes, see [the official proposal][ui-as-code proposal].

[ui-as-code proposal]: https://github.com/dart-lang/language/blob/master/accepted/future-releases/unified-collections/feature-specification.md

**Note: These features are not currently supported in *const* collection
literals. In a future release, we intend to relax this restriction and allow
spread and collection `if` inside const collections.**

### Core library changes

#### `dart:isolate`

*   Added `debugName` property to `Isolate`.
*   Added `debugName` optional parameter to `Isolate.spawn` and
    `Isolate.spawnUri`.

#### `dart:core`

*   RegExp patterns can now use lookbehind assertions.
*   RegExp patterns can now use named capture groups and named backreferences.
    Currently, named group matches can only be retrieved in Dart either by the
    implicit index of the named group or by downcasting the returned Match
    object to the type RegExpMatch. The RegExpMatch interface contains methods
    for retrieving the available group names and retrieving a match by group
    name.

### Dart VM

*   The VM service now requires an authentication code by default. This behavior
    can be disabled by providing the `--disable-service-auth-codes` flag.

*   Support for deprecated flags '-c' and '--checked' has been removed.

### Dart for the Web

#### dart2js

A binary format was added to dump-info. The old JSON format is still available
and provided by default, but we are starting to deprecate it. The new binary
format is more compact and cheaper to generate. On some large apps we tested, it
was 4x faster to serialize and used 6x less memory.

To use the binary format today, use `--dump-info=binary`, instead of
`--dump-info`.

What to expect next?

*   The [visualizer tool][visualizer] will not be updated to support the new
    binary format, but you can find several command-line tools at
    `package:dart2js_info` that provide similar features to those in the
    visualizer.

*   The command-line tools in `package:dart2js_info` also work with the old JSON
    format, so you can start using them even before you enable the new format.

*   In a future release `--dump-info` will default to `--dump-info=binary`. At
    that point, there will be an option to fallback to the JSON format, but the
    visualizer tool will be deprecated.

*   A release after that, the JSON format will no longer be available from
    dart2js, but may be available from a command-line tool in
    `package:dart2js_info`.

[visualizer]: https://dart-lang.github.io/dump-info-visualizer/

### Tools

#### dartfmt

*   Tweak set literal formatting to follow other collection literals.
*   Add support for "UI as code" features.
*   Properly format trailing commas in assertions.
*   Improve indentation of adjacent strings in argument lists.

#### Linter

The Linter was updated to `0.1.86`, which includes the following changes:

*   Added the following lints: `prefer_inlined_adds`,
    `prefer_for_elements_to_map_fromIterable`,
    `prefer_if_elements_to_conditional_expressions`,
    `diagnostic_describe_all_properties`.
*   Updated `file_names` to skip prefixed-extension Dart files (`.css.dart`,
  `.g.dart`, etc.).
*   Fixed false positives in `unnecessary_parenthesis`.

#### Pub

*   Added a CHANGELOG validator that complains if you `pub publish` without
    mentioning the current version.
*   Removed validation of library names when doing `pub publish`.
*   Added support for `pub global activate`ing package from a custom pub URL.
*   Added subcommand: `pub logout`. Logs you out of the current session.

#### Dart native

Initial support for compiling Dart apps to native machine code has been added.
Two new tools have been added to the `bin` folder of the Dart SDK:

* `dart2aot`: AOT (ahead-of-time) compiles a Dart program to native
machine code. The tool is supported on Windows, macOS, and Linux.

* `dartaotruntime`: A small runtime used for executing an AOT compiled program.

## 2.2.0 - 2019-02-26

### Language

Sets now have a literal syntax like lists and maps do:

```dart
var set = {1, 2, 3};
```

Using curly braces makes empty sets ambiguous with maps:

```dart
var collection = {}; // Empty set or map?
```

To avoid breaking existing code, an ambiguous literal is treated as a map.
To create an empty set, you can rely on either a surrounding context type
or an explicit type argument:

```dart
// Variable type forces this to be a set:
Set<int> set = {};

// A single type argument means this must be a set:
var set2 = <int>{};
```

Set literals are released on all platforms. The `set-literals` experiment flag
has been disabled.

### Tools

#### Analyzer

*   The `DEPRECATED_MEMBER_USE` hint was split into two hints:

    *   `DEPRECATED_MEMBER_USE` reports on usage of `@deprecated` members
        declared in a different package.
    *   `DEPRECATED_MEMBER_USE_FROM_SAME_PACKAGE` reports on usage of
        `@deprecated` members declared in the same package.

#### Linter

Upgraded the linter to `0.1.82` which adds the following improvements:

*   Added `provide_deprecation_message`, and
    `use_full_hex_values_for_flutter_colors`, `prefer_null_aware_operators`.
*   Fixed `prefer_const_declarations` set literal false-positives.
*   Updated `prefer_collection_literals` to support set literals.
*   Updated `unnecessary_parenthesis` play nicer with cascades.
*   Removed deprecated lints from the "all options" sample.
*   Stopped registering "default lints".
*   Fixed `hash_and_equals` to respect `hashCode` fields.

### Other libraries

#### `package:kernel`

*   **Breaking change:** The `klass` getter on the `InstanceConstant` class in
    the Kernel AST API has been renamed to `classNode` for consistency.

*   **Breaking change:** Updated `Link` implementation to utilize true symbolic
    links instead of junctions on Windows. Existing junctions will continue to
    work with the new `Link` implementation, but all new links will create
    symbolic links.

    To create a symbolic link, Dart must be run with administrative privileges
    or Developer Mode must be enabled, otherwise a `FileSystemException` will be
    raised with errno set to `ERROR_PRIVILEGE_NOT_HELD` (Issue [33966]).

[33966]: https://github.com/dart-lang/sdk/issues/33966

## 2.1.1 - 2019-02-18

This is a patch version release. Again, the team's focus was mostly on improving
performance and stability after the large changes in Dart 2.0.0. In particular,
dart2js now always uses the "fast startup" emitter and the old emitter has been
removed.

There are a couple of very minor **breaking changes:**

*   In `dart:io`, adding to a closed `IOSink` now throws a `StateError`.

*   On the Dart VM, a soundness hole when using `dart:mirrors` to reflectively
    invoke a method in an incorrect way that violates its static types has
    been fixed (Issue [35611][]).

### Language

This release has no language changes.

### Core library

#### `dart:core`

*   Made `DateTime.parse()` also recognize `,` as a valid decimal separator
    when parsing from a string (Issue [35576][]).

[35576]: https://github.com/dart-lang/sdk/issues/35576

#### `dart:html`

*   Added methods `Element.removeAttribute`, `Element.removeAttributeNS`,
    `Element.hasAttribute` and `Element.hasAttributeNS`. (Issue [35655][]).
*   Improved dart2js compilation of `element.attributes.remove(name)` to
    generate `element.removeAttribute(name)`, so that there is no performance
    reason to migrate to the above methods.
*   Fixed a number of `dart:html` bugs:

    *   Fixed HTML API's with callback typedef to correctly convert Dart
        functions to JS functions (Issue [35484]).
    *   HttpStatus constants exposed in `dart:html` (Issue [34318]).
    *   Expose DomName `ondblclick` and `dblclickEvent` for Angular analyzer.
    *   Fixed `removeAll` on `classes`; `elements` parameter should be
        `Iterable<Object>` to match Set's `removeAll` not `Iterable<E>` (Issue
        [30278]).
    *   Fixed a number of methods on DataTransferItem, Entry, FileEntry and
        DirectoryEntry which previously returned NativeJavaScriptObject.  This
        fixes handling drag/drop of files/directories (Issue [35510]).
    *   Added ability to allow local file access from Chrome browser in ddb.

[35655]: https://github.com/dart-lang/sdk/issues/35655
[30278]: https://github.com/dart-lang/sdk/issues/30278
[34318]: https://github.com/dart-lang/sdk/issues/34318
[35484]: https://github.com/dart-lang/sdk/issues/35484
[35510]: https://github.com/dart-lang/sdk/issues/35510

#### `dart:io`

*   **Breaking Change:** Adding to a closed `IOSink` now throws a `StateError`.
*   Added ability to get and set low level socket options.

[29554]: https://github.com/dart-lang/sdk/issues/29554

### Dart VM

In previous releases it was possible to violate static types using
`dart:mirrors`. This code would run without any TypeErrors and print
"impossible" output:

```dart
import 'dart:mirrors';

class A {
  void method(int v) {
    if (v != null && v is! int) {
      print("This should be impossible: expected null or int got ${v}");
    }
  }
}

void main() {
  final obj = A();
  reflect(obj).invoke(#method, ['not-an-number']);
}
```

This bug is fixed now. Only code that already violates static typing will break.
See Issue [35611][] for more details.

[35611]: https://github.com/dart-lang/sdk/issues/35611

### Dart for the Web

#### dart2js

*   The old "full emitter" back-end is removed and dart2js always uses the "fast
    startup" back-end. The generated fast startup code is optimized to load
    faster, even though it can be slightly larger. The `--fast-startup` and
    `--no-fast-startup` are allowed but ignored. They will be removed in a
    future version.

*   We fixed a bug in how deferred constructor calls were incorrectly not marked
    as deferred. The old behavior didn't cause breakages, but was imprecise and
    pushed more code to the main output unit.

*   A new deferred split algorithm implementation was added.

    This implementation fixes a soundness bug and addresses performance issues
    of the previous implementation, because of that it can have a visible impact
    on apps. In particular:

    *   We fixed a performance issue which was introduced when we migrated to
        the common front-end. On large apps, the fix can cut 2/3 of the time
        spent on this task.

    *   We fixed a bug in how inferred types were categorized (Issue [35311][]).
        The old behavior was unsound and could produce broken programs. The fix
        may cause more code to be pulled into the main output unit.

        This shows up frequently when returning deferred values from closures
        since the closure's inferred return type is the deferred type. For
        example, if you have:

        ```dart
        () async {
          await deferred_prefix.loadLibrary();
          return new deferred_prefix.Foo();
        }
        ```

        The closure's return type is `Future<Foo>`. The old implementation
        defers `Foo`, and incorrectly makes the return type `Future<dynamic>`.
        This may break in places where the correct type is expected.

        The new implementation will not defer `Foo`, and will place it in the
        main output unit. If your intent is to defer it, then you need to ensure
        the return type is not inferred to be `Foo`. For example, you can do so
        by changing the code to a named closure with a declared type, or by
        ensuring that the return expression has the type you want, like:

        ```dart
        () async {
          await deferred_prefix.loadLibrary();
          return new deferred_prefix.Foo() as dynamic;
        }
        ```

        Because the new implementation might require you to inspect and fix your
        app, we exposed two temporary flags:

    *   The `--report-invalid-deferred-types` causes dart2js to run both the
        old and new algorithms and report any cases where an invalid type was
        detected.

    *   The `--new-deferred-split` flag enables this new algorithm.

*   The `--categories=*` flag is being replaced. `--categories=all` was only
    used for testing and it is no longer supported. `--categories=Server`
    continues to work at this time but it is deprecated, please use
    `--server-mode` instead.

*   The `--library-root` flag was replaced by `--libraries-spec`. This flag is
    rarely used by developers invoking dart2js directly. It's important for
    integrating dart2js with build systems. See `--help` for more details on the
    new flag.

[35311]: https://github.com/dart-lang/sdk/issues/35311

### Tools

#### Analyzer

*   Support for `declarations-casts` has been removed and the `implicit-casts`
    option now has the combined semantics of both options. This means that
    users that disable `implicit-casts` might now see errors that were not
    previously being reported.

*   New hints added:

    *   `NON_CONST_CALL_TO_LITERAL_CONSTRUCTOR` and
        `NON_CONST_CALL_TO_LITERAL_CONSTRUCTOR_USING_NEW` inform you when a
        `@literal` const constructor is called in a non-const context (or with
        `new`).
    *   `INVALID_LITERAL_ANNOTATION` reports when something other than a const
        constructor is annotated with `@literal`.
    *   `SUBTYPE_OF_SEALED_CLASS` reports when any class or mixin subclasses
        (extends, implements, mixes in, or constrains to) a `@sealed` class, and
        the two are declared in different packages.
    *   `MIXIN_ON_SEALED_CLASS` reports when a `@sealed` class is used as a
        superclass constraint of a mixin.

#### dartdoc

Default styles now work much better on mobile. Simple browsing and searching of
API docs now work in many cases.

Upgraded the linter to `0.1.78` which adds the following improvements:

*   Added `prefer_final_in_for_each`, `unnecessary_await_in_return`,
    `use_function_type_syntax_for_parameters`,
    `avoid_returning_null_for_future`, and `avoid_shadowing_type_parameters`.
*   Updated `invariant_booleans` status to experimental.
*   Fixed `type_annotate_public_apis` false positives on local functions.
*   Fixed `avoid_shadowing_type_parameters` to report shadowed type parameters
    in generic typedefs.
*   Fixed `use_setters_to_change_properties` to not wrongly lint overriding
    methods.
*   Fixed `cascade_invocations` to not lint awaited targets.
*   Fixed `prefer_conditional_assignment` false positives.
*   Fixed `join_return_with_assignment` false positives.
*   Fixed `cascade_invocations` false positives.
*   Deprecated `prefer_bool_in_asserts` as it is redundant in Dart 2.

## 2.1.0 - 2018-11-15

This is a minor version release. The team's focus was mostly on improving
performance and stability after the large changes in Dart 2.0.0. Notable
changes:

*   We've introduced a dedicated syntax for declaring a mixin. Instead of the
    `class` keyword, it uses `mixin`:

    ```dart
    mixin SetMixin<E> implements Set<E> {
      ...
    }
    ```

    The new syntax also enables `super` calls inside mixins.

*   Integer literals now work in double contexts. When passing a literal number
    to a function that expects a `double`, you no longer need an explicit `.0`
    at the end of the number. In releases before 2.1, you need code like this
    when setting a double like `fontSize`:

    ```dart
    TextStyle(fontSize: 18.0)
    ```

    Now you can remove the `.0`:

    ```dart
    TextStyle(fontSize: 18)
    ```

    In releases before 2.1, `fontSize : 18` causes a static error. This was a
    common mistake and source of friction.

*   **Breaking change:** A number of static errors that should have been
    detected and reported were not supported in 2.0.0. These are reported now,
    which means existing incorrect code may show new errors.

*   `dart:core` now exports `Future` and `Stream`. You no longer need to import
    `dart:async` to use those very common types.

### Language

*   Introduced a new syntax for mixin declarations.

    ```dart
    mixin SetMixin<E> implements Set<E> {
      ...
    }
    ```

    Most classes that are intended to be used as mixins are intended to *only*
    be used as mixins. The library author doesn't want users to be able to
    construct or subclass the class. The new syntax makes that intent clear and
    enforces it in the type system. It is an error to extend or construct a type
    declared using `mixin`. (You can implement it since mixins expose an
    implicit interface.)

    Over time, we expect most mixin declarations to use the new syntax. However,
    if you have a "mixin" class where users *are* extending or constructing it,
    note that moving it to the new syntax is a breaking API change since it
    prevents users from doing that. If you have a type like this that is a
    mixin as well as being a concrete class and/or superclass, then the existing
    syntax is what you want.

    If you need to use a `super` inside a mixin, the new syntax is required.
    This was previously only allowed with the experimental `--supermixins` flag
    because it has some complex interactions with the type system. The new
    syntax addresses those issues and lets you use `super` calls by declaring
    the superclass constraint your mixin requires:

    ```dart
    class Superclass {
      superclassMethod() {
        print("in superclass");
      }
    }

    mixin SomeMixin on Superclass {
      mixinMethod() {
        // This is OK:
        super.superclassMethod();
      }
    }

    class GoodSub extends Superclass with SomeMixin {}

    class BadSub extends Object with SomeMixin {}
    // Error: Since the super() call in mixinMethod() can't find a
    // superclassMethod() to call, this is prohibited.
    ```

    Even if you don't need to use `super` calls, the new mixin syntax is good
    because it clearly expresses that you intend the type to be mixed in.

*   Allow integer literals to be used in double contexts. An integer literal
    used in a place where a double is required is now interpreted as a double
    value. The numerical value of the literal needs to be precisely
    representable as a double value.

*   Integer literals compiled to JavaScript are now allowed to have any value
    that can be exactly represented as a JavaScript `Number`. They were
    previously limited to such numbers that were also representable as signed
    64-bit integers.

**(Breaking)** A number of static errors that should have been detected and
reported were not supported in 2.0.0. These are reported now, which means
existing incorrect code may show new errors:

*   **Setters with the same name as the enclosing class aren't allowed.** (Issue
    [34225][].) It is not allowed to have a class member with the same name as
    the enclosing class:

    ```dart
    class A {
      set A(int x) {}
    }
    ```

    Dart 2.0.0 incorrectly allows this for setters (only). Dart 2.1.0 rejects
    it.

    *To fix:* This is unlikely to break anything, since it violates all style
    guides anyway.

*   **Constant constructors cannot redirect to non-constant constructors.**
    (Issue [34161][].) It is not allowed to have a constant constructor that
    redirects to a non-constant constructor:

    ```dart
    class A {
      const A.foo() : this(); // Redirecting to A()
      A() {}
    }
    ```

    Dart 2.0.0 incorrectly allows this. Dart 2.1.0 rejects it.

    *To fix:* Make the target of the redirection a properly const constructor.

*   **Abstract methods may not unsoundly override a concrete method.** (Issue
    [32014][].) Concrete methods must be valid implementations of their
    interfaces:

    ```dart
    class A {
      num get thing => 2.0;
    }

    abstract class B implements A {
      int get thing;
    }

    class C extends A with B {}
    // 'thing' from 'A' is not a valid override of 'thing' from 'B'.

    main() {
      print(new C().thing.isEven); // Expects an int but gets a double.
    }
    ```

    Dart 2.0.0 allows unsound overrides like the above in some cases. Dart 2.1.0
    rejects them.

    *To fix:* Relax the type of the invalid override, or tighten the type of the
    overridden method.

*   **Classes can't implement FutureOr.** (Issue [33744][].) Dart doesn't allow
    classes to implement the FutureOr type:

    ```dart
    class A implements FutureOr<Object> {}
    ```

    Dart 2.0.0 allows classes to implement FutureOr. Dart 2.1.0 does not.

    *To fix:* Don't do this.

*   **Type arguments to generic typedefs must satisfy their bounds.** (Issue
    [33308][].) If a parameterized typedef specifies a bound, actual arguments
    must be checked against it:

    ```dart
    class A<X extends int> {}

    typedef F<Y extends int> = A<Y> Function();

    F<num> f = null;
    ```

    Dart 2.0.0 allows bounds violations like `F<num>` above. Dart 2.1.0 rejects
    them.

    *To fix:* Either remove the bound on the typedef parameter, or pass a valid
    argument to the typedef.

*   **Constructor invocations must use valid syntax, even with optional `new`.**
    (Issue [34403][].) Type arguments to generic named constructors go after the
    class name, not the constructor name, even when used without an explicit
    `new`:

    ```dart
    class A<T> {
      A.foo() {}
    }

    main() {
      A.foo<String>(); // Incorrect syntax, was accepted in 2.0.0.
      A<String>.foo(); // Correct syntax.
    }
    ```

    Dart 2.0.0 accepts the incorrect syntax when the `new` keyword is left out.
    Dart 2.1.0 correctly rejects this code.

    *To fix:* Move the type argument to the correct position after the class
    name.

*   **Instance members should shadow prefixes.** (Issue [34498][].) If the same
    name is used as an import prefix and as a class member name, then the class
    member name takes precedence in the class scope.

    ```dart
    import 'dart:core';
    import 'dart:core' as core;

    class A {
      core.List get core => null; // "core" refers to field, not prefix.
    }
    ```

    Dart 2.0.0 incorrectly resolves the use of `core` in `core.List` to the
    prefix name. Dart 2.1.0 correctly resolves this to the field name.

    *To fix:* Change the prefix name to something which does not clash with the
    instance member.

*   **Implicit type arguments in extends clauses must satisfy the class
    bounds.** (Issue [34532][].) Implicit type arguments for generic classes are
    computed if not passed explicitly, but when used in an `extends` clause they
    must be checked for validity:

    ```dart
    class Foo<T> {}

    class Bar<T extends Foo<T>> {}

    class Baz extends Bar {} // Should error because Bar completes to Bar<Foo>
    ```

    Dart 2.0.0 accepts the broken code above. Dart 2.1.0 rejects it.

    *To fix:* Provide explicit type arguments to the superclass that satisfy the
    bound for the superclass.

*   **Mixins must correctly override their superclasses.** (Issue [34235][].) In
    some rare cases, combinations of uses of mixins could result in invalid
    overrides not being caught:

    ```dart
    class A {
      num get thing => 2.0;
    }

    class M1 {
      int get thing => 2;
    }

    class B = A with M1;

    class M2 {
      num get thing => 2.0;
    }

    class C extends B with M2 {} // 'thing' from 'M2' not a valid override.

    main() {
      M1 a = new C();
      print(a.thing.isEven); // Expects an int but gets a double.
    }
    ```

    Dart 2.0.0 accepts the above example. Dart 2.1.0 rejects it.

    *To fix:* Ensure that overriding methods are correct overrides of their
    superclasses, either by relaxing the superclass type, or tightening the
    subclass/mixin type.

[32014]: https://github.com/dart-lang/sdk/issues/32014
[33308]: https://github.com/dart-lang/sdk/issues/33308
[33744]: https://github.com/dart-lang/sdk/issues/33744
[34161]: https://github.com/dart-lang/sdk/issues/34161
[34225]: https://github.com/dart-lang/sdk/issues/34225
[34235]: https://github.com/dart-lang/sdk/issues/34235
[34403]: https://github.com/dart-lang/sdk/issues/34403
[34498]: https://github.com/dart-lang/sdk/issues/34498
[34532]: https://github.com/dart-lang/sdk/issues/34532

### Core libraries

#### `dart:async`

*   Fixed a bug where calling `stream.take(0).drain(value)` would not correctly
    forward the `value` through the returned `Future`.
*   Added a `StreamTransformer.fromBind` constructor.
*   Updated `Stream.fromIterable` to send a done event after the error when the
    iterator's `moveNext` throws, and handle if the `current` getter throws
    (issue [33431][]).

[33431]: http://dartbug.com/33431

#### `dart:core`

*   Added `HashMap.fromEntries` and `LinkedHashmap.fromEntries` constructors.
*   Added `ArgumentError.checkNotNull` utility method.
*   Made `Uri` parsing more permissive about `[` and `]` occurring in the path,
    query or fragment, and `#` occurring in fragment.
*   Exported `Future` and `Stream` from `dart:core`.
*   Added operators `&`, `|` and `^` to `bool`.
*   Added missing methods to `UnmodifiableMapMixin`. Some maps intended to
    be unmodifiable incorrectly allowed new methods added in Dart 2 to
    succeed.
*   Deprecated the `provisional` annotation and the `Provisional`
    annotation class. These should have been removed before releasing Dart 2.0,
    and they have no effect.

#### `dart:html`

Fixed Service Workers and any Promise/Future API with a Dictionary parameter.

APIs in dart:html (that take a Dictionary) will receive a Dart Map parameter.
The Map parameter must be converted to a Dictionary before passing to the
browser's API.  Before this change, any Promise/Future API with a
Map/Dictionary parameter never called the Promise and didn't return a Dart
Future - now it does.

This caused a number of breaks especially in Service Workers (register, etc.).
Here is a complete list of the fixed APIs:

*   BackgroundFetchManager
    *   `Future<BackgroundFetchRegistration> fetch(String id, Object requests,
        [Map options])`
*   CacheStorage
    *   `Future match(/*RequestInfo*/ request, [Map options])`
*   CanMakePayment
    *   `Future<List<Client>> matchAll([Map options])`
*   CookieStore
    *   `Future getAll([Map options])`
    *   `Future set(String name, String value, [Map options])`
*   CredentialsContainer
    *   `Future get([Map options])`
    *   `Future create([Map options])`
*   ImageCapture
    *   `Future setOptions(Map photoSettings)`
*   MediaCapabilities
    *   `Future<MediaCapabilitiesInfo> decodingInfo(Map configuration)`
    *   `Future<MediaCapabilitiesInfo> encodingInfo(Map configuration)`
*   MediaStreamTrack
    *   `Future applyConstraints([Map constraints])`
*   Navigator
    *   `Future requestKeyboardLock([List<String> keyCodes])`
    *   `Future requestMidiAccess([Map options])`
    *   `Future share([Map data])`
*   OffscreenCanvas
    *   `Future<Blob> convertToBlob([Map options])`
*   PaymentInstruments
    *   `Future set(String instrumentKey, Map details)`
*   Permissions
    *   `Future<PermissionStatus> query(Map permission)`
    *   `Future<PermissionStatus> request(Map permissions)`
    *   `Future<PermissionStatus> revoke(Map permission)`
*   PushManager
    *   `Future permissionState([Map options])`
    *   `Future<PushSubscription> subscribe([Map options])`
*   RtcPeerConnection
    *   Changed:

        ```dart
        Future createAnswer([options_OR_successCallback,
            RtcPeerConnectionErrorCallback failureCallback,
            Map mediaConstraints])
        ```

        to:

        ```dart
        Future<RtcSessionDescription> createAnswer([Map options])
        ```

    *   Changed:

        ```dart
        Future createOffer([options_OR_successCallback,
            RtcPeerConnectionErrorCallback failureCallback,
            Map rtcOfferOptions])
        ```

        to:

        ```dart
        Future<RtcSessionDescription> createOffer([Map options])
        ```

    *   Changed:

        ```dart
        Future setLocalDescription(Map description,
            VoidCallback successCallback,
            [RtcPeerConnectionErrorCallback failureCallback])
        ```

        to:

        ```dart
        Future setLocalDescription(Map description)
        ```

    *   Changed:

        ```dart
        Future setLocalDescription(Map description,
            VoidCallback successCallback,
            [RtcPeerConnectionErrorCallback failureCallback])
        ```

        to:

        ```dart
        Future setRemoteDescription(Map description)
        ```

*   ServiceWorkerContainer
    *   `Future<ServiceWorkerRegistration> register(String url, [Map options])`
*   ServiceWorkerRegistration
    *   `Future<List<Notification>> getNotifications([Map filter])`
    *   `Future showNotification(String title, [Map options])`
*   VRDevice
    *   `Future requestSession([Map options])`
    *   `Future supportsSession([Map options])`
*   VRSession
    *   `Future requestFrameOfReference(String type, [Map options])`
*   Window
    *   `Future fetch(/*RequestInfo*/ input, [Map init])`
*   WorkerGlobalScope
    *   `Future fetch(/*RequestInfo*/ input, [Map init])`

In addition, exposed Service Worker "self" as a static getter named "instance".
The instance is exposed on four different Service Worker classes and can throw
a InstanceTypeError if the instance isn't of the class expected
(WorkerGlobalScope.instance will always work and not throw):

*   `SharedWorkerGlobalScope.instance`
*   `DedicatedWorkerGlobalScope.instance`
*   `ServiceWorkerGlobalScope.instance`
*   `WorkerGlobalScope.instance`

#### `dart:io`

*   Added new HTTP status codes.

### Dart for the Web

#### dart2js

*   **(Breaking)** Duplicate keys in a const map are not allowed and produce a
    compile-time error. Dart2js used to report this as a warning before. This
    was already an error in dartanalyzer and DDC and will be an error in other
    tools in the future as well.

*   Added `-O` flag to tune optimization levels.  For more details run `dart2js
    -h -v`.

    We recommend to enable optimizations using the `-O` flag instead of
    individual flags for each optimization. This is because the `-O` flag is
    intended to be stable and continue to work in future versions of dart2js,
    while individual flags may come and go.

    At this time we recommend to test and debug with `-O1` and to deploy with
    `-O3`.

### Tool Changes

#### dartfmt

*   Addressed several dartfmt issues when used with the new CFE parser.

#### Linter

Bumped the linter to `0.1.70` which includes the following new lints:

*   `avoid_returning_null_for_void`
*   `sort_pub_dependencies`
*   `prefer_mixin`
*   `avoid_implementing_value_types`
*   `flutter_style_todos`
*   `avoid_void_async`
*   `prefer_void_to_null`

and improvements:

*   Fixed NPE in `prefer_iterable_whereType`.
*   Improved message display for `await_only_futures`
*   Performance improvements for `null_closures`
*   Mixin support
*   Updated `sort_constructors_first` to apply to all members.
*   Updated `unnecessary_this` to work on field initializers.
*   Updated `unawaited_futures` to ignore assignments within cascades.
*   Improved handling of constant expressions with generic type params.
*   NPE fix for `invariant_booleans`.
*   Improved docs for `unawaited_futures`.
*   Updated `unawaited_futures` to check cascades.
*   Relaxed `void_checks` (allowing `T Function()` to be assigned to
    `void Function()`).
*   Fixed false positives in `lines_longer_than_80_chars`.

#### Pub

*   Renamed the `--checked` flag to `pub run` to `--enable-asserts`.
*   Pub will no longer delete directories named "packages".
*   The `--packages-dir` flag is now ignored.

## 2.0.0 - 2018-08-07

This is the first major version release of Dart since 1.0.0, so it contains many
significant changes across all areas of the platform. Large changes include:

*   **(Breaking)** The unsound optional static type system has been replaced
    with a sound static type system using type inference and runtime checks.
    This was formerly called "[strong mode][]" and only used by the Dart for web
    products. Now it is the one official static type system for the entire
    platform and replaces the previous "checked" and "production" modes.

*   **(Breaking)** Functions marked `async` now run synchronously until the
    first `await` statement. Previously, they would return to the event loop
    once at the top of the function body before any code runs ([issue 30345][]).

*   **(Breaking)** Constants in the core libraries have been renamed from
    `SCREAMING_CAPS` to `lowerCamelCase`.

*   **(Breaking)** Many new methods have been added to core library classes. If
    you implement the interfaces of these classes, you will need to implement
    the new methods.

*   **(Breaking)** "dart:isolate" and "dart:mirrors" are no longer supported
    when using Dart for the web. They are still supported in the command-line
    VM.

*   **(Breaking)** Pub's transformer-based build system has been [replaced by a
    new build system][transformers].

*   The `new` keyword is optional and can be omitted. Likewise, `const` can be
    omitted inside a const context ([issue 30921][]).

*   Dartium is no longer maintained or supported.

[issue 30345]: https://github.com/dart-lang/sdk/issues/30345
[issue 30921]: https://github.com/dart-lang/sdk/issues/30921
[strong mode]: https://www.dartlang.org/guides/language/sound-dart
[transformers]: https://www.dartlang.org/tools/pub/obsolete

### Language

*   "[Strong mode][]" is now the official type system of the language.

*   The `new` keyword is optional and can be omitted. Likewise, `const` can be
    omitted inside a const context.

*   A string in a `part of` declaration may now be used to refer to the library
    this file is part of. A library part can now declare its library as either:

    ```dart
    part of name.of.library;
    ```

    Or:

    ```dart
    part of "uriReferenceOfLibrary.dart";
    ```

    This allows libraries with no library declarations (and therefore no name)
    to have parts, and it allows tools to easily find the library of a part
    file. The Dart 1.0 syntax is supported but deprecated.

*   Functions marked `async` now run synchronously until the first `await`
    statement. Previously, they would return to the event loop once at the top
    of the function body before any code runs ([issue 30345][]).

*   The type `void` is now a Top type like `dynamic`, and `Object`. It also now
    has new errors for being used where not allowed (such as being assigned to
    any non-`void`-typed parameter). Some libraries (importantly, mockito) may
    need to be updated to accept void values to keep their APIs working.

*   Future flattening is now done only as specified in the Dart 2.0 spec, rather
    than more broadly. This means that the following code has an error on the
    assignment to `y`.

    ```dart
    test() {
      Future<int> f;
      var x = f.then<Future<List<int>>>((x) => []);
      Future<List<int>> y = x;
    }
    ```

*   Invocations of `noSuchMethod()` receive default values for optional args.
    The following program used to print "No arguments passed", and now prints
    "First argument is 3".

    ```dart
    abstract class B {
      void m([int x = 3]);
    }

    class A implements B {
      noSuchMethod(Invocation i) {
        if (i.positionalArguments.length == 0) {
          print("No arguments passed");
        } else {
          print("First argument is ${i.positionalArguments[0]}");
        }
      }
    }

    void main() {
      A().m();
    }
    ```

*   Bounds on generic functions are invariant. The following program now issues
    an invalid override error ([issue 29014][sdk#29014]):

    ```dart
    class A {
      void f<T extends int>() {}
    }

    class B extends A {
      @override
      void f<T extends num>() {}
    }
    ```

*   Numerous corner case bugs around return statements in synchronous and
    asynchronous functions fixed. Specifically:

    *   Issues [31887][issue 31887], [32881][issue 32881]. Future flattening
        should not be recursive.
    *   Issues [30638][issue 30638], [32233][issue 32233]. Incorrect downcast
        errors with `FutureOr`.
    *   Issue [32233][issue 32233]. Errors when returning `FutureOr`.
    *   Issue [33218][issue 33218]. Returns in functions with void related
        types.
    *   Issue [31278][issue 31278]. Incorrect hint on empty returns in async.
        functions.

*   An empty `return;` in an async function with return type `Future<Object>`
    does not report an error.

*   `return exp;` where `exp` has type `void` in an async function is now an
    error unless the return type of the function is `void` or `dynamic`.

*   Mixed return statements of the form `return;` and `return exp;` are now
    allowed when `exp` has type `void`.

*   A compile time error is emitted for any literal which cannot be exactly
    represented on the target platform. As a result, dart2js and DDC report
    errors if an integer literal cannot be represented exactly in JavaScript
    ([issue 33282][]).

*   New member conflict rules have been implemented. Most cases of conflicting
    members with the same name are now static errors ([issue 33235][]).

[sdk#29014]: https://github.com/dart-lang/sdk/issues/29014
[issue 30638]: https://github.com/dart-lang/sdk/issues/30638
[issue 31278]: https://github.com/dart-lang/sdk/issues/31278
[issue 31887]: https://github.com/dart-lang/sdk/issues/31887
[issue 32233]: https://github.com/dart-lang/sdk/issues/32233
[issue 32881]: https://github.com/dart-lang/sdk/issues/32881
[issue 33218]: https://github.com/dart-lang/sdk/issues/33218
[issue 33235]: https://github.com/dart-lang/sdk/issues/33235
[issue 33282]: https://github.com/dart-lang/sdk/issues/33282
[issue 33341]: https://github.com/dart-lang/sdk/issues/33341

### Core libraries

*   Replaced `UPPER_CASE` constant names with `lowerCamelCase`. For example,
    `HTML_ESCAPE` is now `htmlEscape`.

*   The Web libraries were re-generated using Chrome 63 WebIDLs
    ([details][idl]).

[idl]: https://github.com/dart-lang/sdk/wiki/Chrome-63-Dart-Web-Libraries

#### `dart:async`

*   `Stream`:
    *   Added `cast` and `castFrom`.
    *   Changed `firstWhere`, `lastWhere`, and `singleWhere` to return
        `Future<T>` and added an optional `T orElse()` callback.
*   `StreamTransformer`: added `cast` and `castFrom`.
*   `StreamTransformerBase`: new class.
*   `Timer`: added `tick` property.
*   `Zone`
    *   changed to be strong-mode clean. This required some breaking API
        changes. See https://goo.gl/y9mW2x for more information.
    *   Added `bindBinaryCallbackGuarded`, `bindCallbackGuarded`, and
        `bindUnaryCallbackGuarded`.
    *   Renamed `Zone.ROOT` to `Zone.root`.
*   Removed the deprecated `defaultValue` parameter on `Stream.firstWhere` and
    `Stream.lastWhere`.
*   Changed an internal lazily-allocated reusable "null future" to always belong
    to the root zone. This avoids race conditions where the first access to the
    future determined which zone it would belong to. The zone is only used for
    *scheduling* the callback of listeners, the listeners themselves will run in
    the correct zone in any case. Issue [#32556](http://dartbug.com/32556).

#### `dart:cli`

*   *New* "provisional" library for CLI-specific features.
*   `waitFor`: function that suspends a stack to wait for a `Future` to
    complete.

#### `dart:collection`

*   `MapBase`: added `mapToString`.
*   `LinkedHashMap` no longer implements `HashMap`
*   `LinkedHashSet` no longer implements `HashSet`.
*   Added `of` constructor to `Queue`, `ListQueue`, `DoubleLinkedQueue`,
    `HashSet`, `LinkedHashSet`, `SplayTreeSet`, `Map`, `HashMap`,
    `LinkedHashMap`, `SplayTreeMap`.
*   Removed `Maps` class. Extend `MapBase` or mix in `MapMixin` instead to
    provide map method implementations for a class.
*   Removed experimental `Document` method `getCSSCanvasContext` and property
    `supportsCssCanvasContext`.
*   Removed obsolete `Element` property `xtag` no longer supported in browsers.
*   Exposed `ServiceWorker` class.
*   Added constructor to `MessageChannel` and `MessagePort` `addEventListener`
    automatically calls `start` method to receive queued messages.

#### `dart:convert`

*   `Base64Codec.decode` return type is now `Uint8List`.
*   `JsonUnsupportedObjectError`: added `partialResult` property
*   `LineSplitter` now implements `StreamTransformer<String, String>` instead of
    `Converter`. It retains `Converter` methods `convert` and
    `startChunkedConversion`.
*   `Utf8Decoder` when compiled with dart2js uses the browser's `TextDecoder` in
    some common cases for faster decoding.
*   Renamed `ASCII`, `BASE64`, `BASE64URI`, `JSON`, `LATIN1` and `UTF8` to
    `ascii`, `base64`, `base64Uri`, `json`, `latin1` and `utf8`.
*   Renamed the `HtmlEscapeMode` constants `UNKNOWN`, `ATTRIBUTE`,
    `SQ_ATTRIBUTE` and `ELEMENT` to `unknown`, `attribute`, `sqAttribute` and
    `elements`.
*   Added `jsonEncode`, `jsonDecode`, `base64Encode`, `base64UrlEncode` and
    `base64Decode` top-level functions.
*   Changed return type of `encode` on `AsciiCodec` and `Latin1Codec`, and
    `convert` on `AsciiEncoder`, `Latin1Encoder`, to `Uint8List`.
*   Allow `utf8.decoder.fuse(json.decoder)` to ignore leading Unicode BOM.

#### `dart:core`

*   `BigInt` class added to support integers greater than 64-bits.
*   Deprecated the `proxy` annotation.
*   Added `Provisional` class and `provisional` field.
*   Added `pragma` annotation.
*   `RegExp` added static `escape` function.
*   The `Uri` class now correctly handles paths while running on Node.js on
    Windows.
*   Core collection changes:
    *   `Iterable` added members `cast`, `castFrom`, `followedBy` and
        `whereType`.
    *   `Iterable.singleWhere` added `orElse` parameter.
    *   `List` added `+` operator, `first` and `last` setters, and `indexWhere`
        and `lastIndexWhere` methods, and static `copyRange` and `writeIterable`
        methods.
    *   `Map` added `fromEntries` constructor.
    *   `Map` added `addEntries`, `cast`, `entries`, `map`, `removeWhere`,
        `update` and `updateAll` members.
    *   `MapEntry`: new class used by `Map.entries`.
    *   *Note*: if a class extends `IterableBase`, `ListBase`, `SetBase` or
        `MapBase` (or uses the corresponding mixins) from `dart:collection`, the
        new members are implemented automatically.
    *   Added `of` constructor to `List`, `Set`, `Map`.
*   Renamed `double.INFINITY`, `double.NEGATIVE_INFINITY`, `double.NAN`,
    `double.MAX_FINITE` and `double.MIN_POSITIVE` to `double.infinity`,
    `double.negativeInfinity`, `double.nan`, `double.maxFinite` and
    `double.minPositive`.
*   Renamed the following constants in `DateTime` to lower case: `MONDAY`
    through `SUNDAY`, `DAYS_PER_WEEK` (as `daysPerWeek`), `JANUARY` through
    `DECEMBER` and `MONTHS_PER_YEAR` (as `monthsPerYear`).
*   Renamed the following constants in `Duration` to lower case:
    `MICROSECONDS_PER_MILLISECOND` to `microsecondsPerMillisecond`,
    `MILLISECONDS_PER_SECOND` to `millisecondsPerSecond`, `SECONDS_PER_MINUTE`
    to `secondsPerMinute`, `MINUTES_PER_HOUR` to `minutesPerHour`,
    `HOURS_PER_DAY` to `hoursPerDay`, `MICROSECONDS_PER_SECOND` to
    `microsecondsPerSecond`, `MICROSECONDS_PER_MINUTE` to
    `microsecondsPerMinute`, `MICROSECONDS_PER_HOUR` to `microsecondsPerHour`,
    `MICROSECONDS_PER_DAY` to `microsecondsPerDay`, `MILLISECONDS_PER_MINUTE` to
    `millisecondsPerMinute`, `MILLISECONDS_PER_HOUR` to `millisecondsPerHour`,
    `MILLISECONDS_PER_DAY` to `millisecondsPerDay`, `SECONDS_PER_HOUR` to
    `secondsPerHour`, `SECONDS_PER_DAY` to `secondsPerDay`, `MINUTES_PER_DAY` to
    `minutesPerDay`, and `ZERO` to `zero`.
*   Added `typeArguments` to `Invocation` class.
*   Added constructors to invocation class that allows creation of `Invocation`
    objects directly, without going through `noSuchMethod`.
*   Added `unaryMinus` and `empty` constant symbols on the `Symbol` class.
*   Changed return type of `UriData.dataAsBytes` to `Uint8List`.
*   Added `tryParse` static method to `int`, `double`, `num`, `BigInt`, `Uri`
    and `DateTime`.
*   Deprecated `onError` parameter on `int.parse`, `double.parse` and
    `num.parse`.
*   Deprecated the `NoSuchMethodError` constructor.
*   `int.parse` on the VM no longer accepts unsigned hexadecimal numbers greater
    than or equal to `2**63` when not prefixed by `0x`. (SDK issue
    [32858](https://github.com/dart-lang/sdk/issues/32858))

#### `dart:developer`

*   `Flow` class added.
*   `Timeline.startSync` and `Timeline.timeSync` now accepts an optional
    parameter `flow` of type `Flow`. The `flow` parameter is used to generate
    flow timeline events that are enclosed by the slice described by
    `Timeline.{start,finish}Sync` and `Timeline.timeSync`.

<!--
Still need entries for all changes to dart:html since 1.x
-->

#### `dart:html`

*   Removed deprecated `query` and `queryAll`. Use `querySelector` and
    `querySelectorAll`.

#### `dart:io`

*   `HttpStatus` added `UPGRADE_REQUIRED`.
*   `IOOverrides` and `HttpOverrides` added to aid in writing tests that wish to
    mock varios `dart:io` objects.
*   `Platform.operatingSystemVersion` added  that gives a platform-specific
    String describing the version of the operating system.
*   `ProcessStartMode.INHERIT_STDIO` added, which allows a child process to
    inherit the parent's stdio handles.
*   `RawZLibFilter` added  for low-level access to compression and decompression
    routines.
*   Unified backends for `SecureSocket`, `SecurityContext`, and
    `X509Certificate` to be consistent across all platforms. All `SecureSocket`,
    `SecurityContext`, and `X509Certificate` properties and methods are now
    supported on iOS and OSX.
*   `SecurityContext.alpnSupported` deprecated as ALPN is now supported on all
    platforms.
*   `SecurityContext`: added `withTrustedRoots` named optional parameter
    constructor, which defaults to false.
*   Added a `timeout` parameter to `Socket.connect`, `RawSocket.connect`,
    `SecureSocket.connect` and `RawSecureSocket.connect`. If a connection
    attempt takes longer than the duration specified in `timeout`, a
    `SocketException` will be thrown. Note: if the duration specified in
    `timeout` is greater than the OS level timeout, a timeout may occur sooner
    than specified in `timeout`.
*   `Stdin.hasTerminal` added, which is true if stdin is attached to a terminal.
*   `WebSocket` added static `userAgent` property.
*   `RandomAccessFile.close` returns `Future<void>`
*   Added `IOOverrides.socketConnect`.
*   Added Dart-styled constants to  `ZLibOptions`, `FileMode`, `FileLock`,
    `FileSystemEntityType`, `FileSystemEvent`, `ProcessStartMode`,
    `ProcessSignal`, `InternetAddressType`, `InternetAddress`,
    `SocketDirection`, `SocketOption`, `RawSocketEvent`, and `StdioType`, and
    deprecated the old `SCREAMING_CAPS` constants.
*   Added the Dart-styled top-level constants `zlib`, `gzip`, and
    `systemEncoding`, and deprecated the old `SCREAMING_CAPS` top-level
    constants.
*   Removed the top-level `FileMode` constants `READ`, `WRITE`, `APPEND`,
    `WRITE_ONLY`, and `WRITE_ONLY_APPEND`. Please use e.g. `FileMode.read`
    instead.
*   Added `X509Certificate.der`, `X509Certificate.pem`, and
    `X509Certificate.sha1`.
*   Added `FileSystemEntity.fromRawPath` constructor to allow for the creation
    of `FileSystemEntity` using `Uint8List` buffers.
*   Dart-styled constants have been added for `HttpStatus`, `HttpHeaders`,
    `ContentType`, `HttpClient`, `WebSocketStatus`, `CompressionOptions`, and
    `WebSocket`. The `SCREAMING_CAPS` constants are marked deprecated. Note that
    `HttpStatus.CONTINUE` is now `HttpStatus.continue_`, and that e.g.
    `HttpHeaders.FIELD_NAME` is now `HttpHeaders.fieldNameHeader`.
*   Deprecated `Platform.packageRoot`, which is only used for `packages/`
    directory resolution which is no longer supported. It will now always return
    null, which is a value that was always possible for it to return previously.
*   Adds `HttpClient.connectionTimeout`.
*   Adds `{Socket,RawSocket,SecureSocket}.startConnect`. These return a
    `ConnectionTask`, which can be used to cancel an in-flight connection
    attempt.

#### `dart:isolate`

*   Make `Isolate.spawn` take a type parameter representing the argument type of
    the provided function. This allows functions with arguments types other than
    `Object` in strong mode.
*   Rename `IMMEDIATE` and `BEFORE_NEXT_EVENT` on `Isolate` to `immediate` and
    `beforeNextEvent`.
*   Deprecated `Isolate.packageRoot`, which is only used for `packages/`
    directory resolution which is no longer supported. It will now always return
    null, which is a value that was always possible for it to return previously.
*   Deprecated `packageRoot` parameter in `Isolate.spawnUri`, which is was
    previously used only for `packages/` directory resolution. That style of
    resolution is no longer supported in Dart 2.

<!--
Still need entries for all changes to dart:js since 1.x
-->

#### `dart.math`

*   Renamed `E`, `LN10`, `LN`, `LOG2E`, `LOG10E`, `PI`, `SQRT1_2` and `SQRT2` to
    `e`, `ln10`, `ln`, `log2e`, `log10e`, `pi`, `sqrt1_2` and `sqrt2`.

#### `dart.mirrors`

*   Added `IsolateMirror.loadUri`, which allows dynamically loading additional
    code.
*   Marked `MirrorsUsed` as deprecated. The `MirrorsUsed` annotation was only
    used to inform the dart2js compiler about how mirrors were used, but dart2js
    no longer supports the mirrors library altogether.

<!--
Still need entries for all changes to dart:svg since 1.x
-->

#### `dart:typed_data`

*   Added `Unmodifiable` view classes over all `List` types.
*   Renamed `BYTES_PER_ELEMENT` to `bytesPerElement` on all typed data lists.
*   Renamed constants `XXXX` through `WWWW` on `Float32x4` and `Int32x4` to
    lower-case `xxxx` through `wwww`.
*   Renamed `Endinanness` to `Endian` and its constants from `BIG_ENDIAN`,
    `LITTLE_ENDIAN` and `HOST_ENDIAN` to `little`, `big` and `host`.

<!--
Still need entries for all changes to dart:web_audio,web_gl,web_sql since 1.x
-->

### Dart VM

*   Support for MIPS has been removed.

*   Dart `int` is now restricted to 64 bits. On overflow, arithmetic operations
    wrap around, and integer literals larger than 64 bits are not allowed. See
    https://github.com/dart-lang/sdk/blob/master/docs/language/informal/int64.md
    for details.

*   The Dart VM no longer attempts to perform `packages/` directory resolution
    (for loading scripts, and in `Isolate.resolveUri`). Users relying on
    `packages/` directories should switch to `.packages` files.

### Dart for the Web

*   Expose JavaScript Promise APIs using Dart futures. For example,
    `BackgroundFetchManager.get` is defined as:

    ```dart
      Future<BackgroundFetchRegistration> get(String id)
    ```

    It can be used like:

    ```dart
    BackgroundFetchRegistration result = await fetchMgr.get('abc');
    ```

    The underlying JS Promise-to-Future mechanism will be exposed as a public
    API in the future.

#### Dart Dev Compiler (DDC)

*   dartdevc will no longer throw an error from `is` checks that return a
    different result in weak mode (SDK [issue 28988][sdk#28988]). For example:

    ```dart
    main() {
      List l = [];
      // Prints "false", does not throw.
      print(l is List<String>);
    }
    ```

*   Failed `as` casts on `Iterable<T>`, `Map<T>`, `Future<T>`, and `Stream<T>`
    are no longer ignored. These failures were ignored to make it easier to
    migrate Dart 1 code to strong mode, but ignoring them is a hole in the type
    system. This closes part of that hole. (We still need to stop ignoring "as"
    cast failures on function types, and implicit cast failures on the above
    types and function types.)

[sdk#28988]: https://github.com/dart-lang/sdk/issues/28988

#### dart2js

*   dart2js now compiles programs with Dart 2.0 semantics. Apps are expected to
    be bigger than before, because Dart 2.0 has many more implicit checks
    (similar to the `--checked` flag in Dart 1.0).

    We exposed a `--omit-implicit-checks` flag which removes most of the extra
    implicit checks. Only use this if you have enough test coverage to know that
    the app will work well without the checks. If a check would have failed and
    it is omitted, your app may crash or behave in unexpected ways. This flag is
    similar to `--trust-type-annotations` in Dart 1.0.

*   dart2js replaced its front-end with the common front-end (CFE). Thanks to
    the CFE, dart2js errors are more consistent with all other Dart tools.

*   dart2js replaced its source-map implementation.  There aren't any big
    differences, but more data is emitted for synthetic code generated by the
    compiler.

*   `dart:mirrors` support was removed. Frameworks are encouraged to use
    code-generation instead. Conditional imports indicate that mirrors are not
    supported, and any API in the mirrors library will throw at runtime.

*   The generated output of dart2js can now be run as a webworker.

*   `dart:isolate` support was removed. To launch background tasks, please
    use webworkers instead. APIs for webworkers can be accessed from `dart:html`
    or JS-interop.

*   dart2js no longer supports the `--package-root` flag. This flag was
    deprecated in favor of `--packages` long ago.

### Tool Changes

#### Analyzer

*   The analyzer will no longer issue a warning when a generic type parameter is
    used as the type in an instance check. For example:

    ```dart
    test<T>() {
      print(3 is T); // No warning
    }
    ```

*   New static checking of `@visibleForTesting` elements. Accessing a method,
    function, class, etc. annotated with `@visibleForTesting` from a file _not_
    in a `test/` directory will result in a new hint ([issue 28273][]).

*   Static analysis now respects functions annotated with `@alwaysThrows`
    ([issue 31384][]).

*   New hints added:

    *   `NULL_AWARE_BEFORE_OPERATOR` when an operator is used after a null-aware
        access. For example:

        ```dart
        x?.a - ''; // HINT
        ```

    *   `NULL_AWARE_IN_LOGICAL_OPERATOR` when an expression with null-aware
        access is used as a condition in logical operators. For example:

        ```dart
        x.a || x?.b; // HINT
        ```

*   The command line analyzer (dartanalyzer) and the analysis server no longer
    treat directories named `packages` specially. Previously they had ignored
    these directories - and their contents - from the point of view of analysis.
    Now they'll be treated just as regular directories. This special-casing of
    `packages` directories was to support using symlinks for package:
    resolution; that functionality is now handled by `.packages` files.

*   New static checking of duplicate shown or hidden names in an export
    directive ([issue 33182][]).

*   The analysis server will now only analyze code in Dart 2 mode ('strong
    mode'). It will emit warnings for analysis options files that have
    `strong-mode: false` set (and will emit a hint for `strong-mode: true`,
    which is no longer necessary).

*   The dartanalyzer `--strong` flag is now deprecated and ignored. The
    command-line analyzer now only analyzes code in strong mode.

[issue 28273]: https://github.com/dart-lang/sdk/issues/28273
[issue 31384]: https://github.com/dart-lang/sdk/issues/31384
[issue 33182]: https://github.com/dart-lang/sdk/issues/33182

#### dartfmt

*   Support `assert()` in const constructor initializer lists.

*   Better formatting for multi-line strings in argument lists.

*   Force splitting an empty block as the then body of an if with an else.

*   Support metadata annotations on enum cases.

*   Add `--fix` to remove unneeded `new` and `const` keywords, and change `:` to
    `=` before named parameter default values.

*   Change formatting rules around static methods to uniformly format code with
    and without `new` and `const`.

*   Format expressions inside string interpolation.

#### Pub

*   Pub has a brand new version solver! It supports all the same features as the
    old version solver, but it's much less likely to stall out on difficult
    package graphs, and it's much clearer about why a solution can't be found
    when version solving fails.

*   Remove support for transformers, `pub build`, and `pub serve`. Use the
    [new build system][transformers] instead.

*   There is now a default SDK constraint of `<2.0.0` for any package with no
    existing upper bound. This allows us to move more safely to 2.0.0. All new
    packages published on pub will now require an upper bound SDK constraint so
    future major releases of Dart don't destabilize the package ecosystem.

    All SDK constraint exclusive upper bounds are now treated as though they
    allow pre-release versions of that upper bound. For example, the SDK
    constraint `>=1.8.0 <2.0.0` now allows pre-release SDK versions such as
    `2.0.0-beta.3.0`. This allows early adopters to try out packages that don't
    explicitly declare support for the new version yet. You can disable this
    functionality by setting the `PUB_ALLOW_PRERELEASE_SDK` environment variable
    to `false`.

*   Allow depending on a package in a subdirectory of a Git repository. Git
    dependencies may now include a `path` parameter, indicating that the package
    exists in a subdirectory of the Git repository. For example:

    ```yaml
    dependencies:
      foobar:
        git:
          url: git://github.com/dart-lang/multi_package_repo
          path: pkg/foobar
    ```

*   Added an `--executables` option to `pub deps` command. This will list all
    available executables that can be run with `pub run`.

*   The Flutter `sdk` source will now look for packages in
    `flutter/bin/cache/pkg/` as well as `flutter/packages/`. In particular, this
    means that packages can depend on the `sky_engine` package from the `sdk`
    source ([issue 1775][pub#1775]).

*   Pub now caches compiled packages and snapshots in the `.dart_tool/pub`
    directory, rather than the `.pub` directory ([issue 1795][pub#1795]).

*   Other bug fixes and improvements.

[issue 30246]: https://github.com/dart-lang/sdk/issues/30246
[pub#1679]: https://github.com/dart-lang/pub/issues/1679
[pub#1684]: https://github.com/dart-lang/pub/issues/1684
[pub#1775]: https://github.com/dart-lang/pub/issues/1775
[pub#1795]: https://github.com/dart-lang/pub/issues/1795
[pub#1823]: https://github.com/dart-lang/pub/issues/1823

## 1.24.3 - 2017-12-14

* Fix for constructing a new SecurityContext that contains the built-in
  certificate authority roots
  ([issue 24693](https://github.com/dart-lang/sdk/issues/24693)).

### Core library changes

* `dart:io`
  * Unified backends for `SecureSocket`, `SecurityContext`, and
    `X509Certificate` to be consistent across all platforms. All
    `SecureSocket`, `SecurityContext`, and `X509Certificate` properties and
    methods are now supported on iOS and OSX.

## 1.24.2 - 2017-06-22

* Fixes for debugging in Dartium.
  * Fix DevConsole crash with JS
    ([issue 29873](https://github.com/dart-lang/sdk/issues/29873)).
  * Fix debugging in WebStorm, NULL returned for JS objects
    ([issue 29854](https://github.com/dart-lang/sdk/issues/29854)).

## 1.24.1 - 2017-06-14

* Bug fixes for dartdevc support in `pub serve`.
  * Fixed module config invalidation logic so modules are properly
    recalculated when package layout changes.
  * Fixed exception when handling require.js errors that aren't script load
    errors.
  * Fixed an issue where requesting the bootstrap.js file before the dart.js
    file would result in a 404.
  * Fixed a Safari issue during bootstrapping (note that Safari is still not
    officially supported but does work for trivial examples).
* Fix for a Dartium issue where there was no sound in checked mode
  ([issue 29810](https://github.com/dart-lang/sdk/issues/29810)).

## 1.24.0 - 2017-06-12

### Language
* During a dynamic type check, `void` is not required to be `null` anymore.
  In practice, this makes overriding `void` functions with non-`void` functions
  safer.

* During static analysis, a function or setter declared using `=>` with return
  type `void` now allows the returned expression to have any type. For example,
  assuming the declaration `int x;`, it is now type correct to have
  `void f() => ++x;`.

* A new function-type syntax has been added to the language.
  **Warning**: *In Dart 1.24, this feature is incomplete, and not stable in the Analyzer.*

  Intuitively, the type of a function can be constructed by textually replacing
  the function's name with `Function` in its declaration. For instance, the
  type of `void foo() {}` would be `void Function()`. The new syntax may be used
  wherever a type can be written. It is thus now possible to declare fields
  containing functions without needing to write typedefs: `void Function() x;`.
  The new function type has one restriction: it may not contain the old-style
  function-type syntax for its parameters. The following is thus illegal:
  `void Function(int f())`.
  `typedefs` have been updated to support this new syntax.

  Examples:

  ```dart
  typedef F = void Function();  // F is the name for a `void` callback.
  int Function(int) f;  // A field `f` that contains an int->int function.

  class A<T> {
    // The parameter `callback` is a function that takes a `T` and returns
    // `void`.
    void forEach(void Function(T) callback);
  }

  // The new function type supports generic arguments.
  typedef Invoker = T Function<T>(T Function() callback);
  ```

### Core library changes

* `dart:async`, `dart:core`, `dart:io`
    * Adding to a closed sink, including `IOSink`, is no longer not allowed. In
      1.24, violations are only reported (on stdout or stderr), but a future
      version of the Dart SDK will change this to throwing a `StateError`.

* `dart:convert`
  * **BREAKING** Removed the deprecated `ChunkedConverter` class.
  * JSON maps are now typed as `Map<String, dynamic>` instead of
    `Map<dynamic, dynamic>`. A JSON-map is not a `HashMap` or `LinkedHashMap`
    anymore (but just a `Map`).

* `dart:io`
  * Added `Platform.localeName`, needed for accessing the locale on platforms
    that don't store it in an environment variable.
  * Added `ProcessInfo.currentRss` and `ProcessInfo.maxRss` for inspecting
    the Dart VM process current and peak resident set size.
  * Added `RawSynchronousSocket`, a basic synchronous socket implementation.

* `dart:` web APIs have been updated to align with Chrome v50.
   This change includes **a large number of changes**, many of which are
   breaking. In some cases, new class names may conflict with names that exist
   in existing code.

* `dart:html`

  * **REMOVED** classes: `Bluetooth`, `BluetoothDevice`,
    `BluetoothGattCharacteristic`, `BluetoothGattRemoteServer`,
    `BluetoothGattService`, `BluetoothUuid`, `CrossOriginConnectEvent`,
    `DefaultSessionStartEvent`, `DomSettableTokenList`, `MediaKeyError`,
    `PeriodicSyncEvent`, `PluginPlaceholderElement`, `ReadableStream`,
    `StashedMessagePort`, `SyncRegistration`

  * **REMOVED** members:
    * `texImage2DCanvas` was removed from `RenderingContext`.
    * `endClip` and `startClip` were removed from `Animation`.
    * `after` and `before` were removed from `CharacterData`, `ChildNode` and
      `Element`.
    * `keyLocation` was removed from `KeyboardEvent`. Use `location` instead.
    * `generateKeyRequest`, `keyAddedEvent`, `keyErrorEvent`, `keyMessageEvent`,
      `mediaGroup`, `needKeyEvent`, `onKeyAdded`, `onKeyError`, `onKeyMessage`,
      and `onNeedKey` were removed from `MediaElement`.
    * `getStorageUpdates` was removed from `Navigator`
    * `status` was removed from `PermissionStatus`
    * `getAvailability` was removed from `PreElement`

  * Other behavior changes:
    * URLs returned in CSS or html are formatted with quoted string.
      Like `url("http://google.com")` instead of `url(http://google.com)`.
    * Event timestamp property type changed from `int` to `num`.
    * Chrome introduced slight layout changes of UI objects.
      In addition many height/width dimensions are returned in subpixel values
      (`num` instead of whole numbers).
    * `setRangeText` with a `selectionMode` value of 'invalid' is no longer
      valid. Only "select", "start", "end", "preserve" are allowed.

* `dart:svg`

  * A large number of additions and removals. Review your use of `dart:svg`
    carefully.

* `dart:web_audio`

  * new method on `AudioContext` - `createIirFilter` returns a new class
    `IirFilterNode`.

* `dart:web_gl`

  * new classes: `CompressedTextureAstc`, `ExtColorBufferFloat`,
    `ExtDisjointTimerQuery`, and `TimerQueryExt`.

  * `ExtFragDepth` added: `readPixels2` and `texImage2D2`.

#### Strong Mode

* Removed ad hoc `Future.then` inference in favor of using `FutureOr`.  Prior to
  adding `FutureOr` to the language, the analyzer implemented an ad hoc type
  inference for `Future.then` (and overrides) treating it as if the onValue
  callback was typed to return `FutureOr` for the purposes of inference.
  This ad hoc inference has been removed now that `FutureOr` has been added.

  Packages that implement `Future` must either type the `onValue` parameter to
  `.then` as returning `FutureOr<T>`, or else must leave the type of the parameter
  entirely to allow inference to fill in the type.

* During static analysis, a function or setter declared using `=>` with return
  type `void` now allows the returned expression to have any type.

### Tool Changes

* Dartium

  Dartium is now based on Chrome v50. See *Core library changes* above for
  details on the changed APIs.

* Pub

  * `pub build` and `pub serve`

    * Added support for the Dart Development Compiler.

      Unlike dart2js, this new compiler is modular, which allows pub to do
      incremental re-builds for `pub serve`, and potentially `pub build` in the
      future.

      In practice what that means is you can edit your Dart files, refresh in
      Chrome (or other supported browsers), and see your edits almost
      immediately. This is because pub is only recompiling your package, not all
      packages that you depend on.

      There is one caveat with the new compiler, which is that your package and
      your dependencies must all be strong mode clean. If you are getting an
      error compiling one of your dependencies, you will need to file bugs or
      send pull requests to get them strong mode clean.

      There are two ways of opting into the new compiler:

        * Use the new `--web-compiler` flag, which supports `dartdevc`,
          `dart2js` or `none` as options. This is the easiest way to try things
          out without changing the default.

        * Add config to your pubspec. There is a new `web` key which supports a
          single key called `compiler`. This is a map from mode names to
          compiler to use. For example, to default to dartdevc in debug mode you
          can add the following to your pubspec:

          ```yaml
          web:
            compiler:
              debug: dartdevc
          ```

      You can also use the new compiler to run your tests in Chrome much more
      quickly than you can with dart2js. In order to do that, run
      `pub serve test --web-compiler=dartdevc`, and then run
      `pub run test -p chrome --pub-serve=8080`.

    * The `--no-dart2js` flag has been deprecated in favor of
      `--web-compiler=none`.

    * `pub build` will use a failing exit code if there are errors in any
      transformer.

  * `pub publish`

    * Added support for the UNLICENSE file.

    * Packages that depend on the Flutter SDK may be published.

  * `pub get` and `pub upgrade`

    * Don't dump a stack trace when a network error occurs while fetching
      packages.

* dartfmt
    * Preserve type parameters in new generic function typedef syntax.
    * Add self-test validation to ensure formatter bugs do not cause user code
      to be lost.

### Infrastructure changes

* As of this release, we'll show a warning when using the MIPS architecture.
  Unless we learn about any critical use of Dart on MIPS in the meantime, we're
  planning to deprecate support for MIPS starting with the next stable release.

## 1.23.0 - 2017-04-21

#### Strong Mode

* Breaking change - it is now a strong mode error if a mixin causes a name
  conflict between two private members (field/getter/setter/method) from a
  different library. (SDK
  issue [28809](https://github.com/dart-lang/sdk/issues/28809)).

lib1.dart:


```dart
class A {
  int _x;
}

class B {
  int _x;
}
```

lib2.dart:


```dart
import 'lib1.dart';

class C extends A with B {}
```

```
    error • The private name _x, defined by B, conflicts with the same name defined by A at tmp/lib2.dart:3:24 • private_collision_in_mixin_application
```


* Breaking change - strong mode will prefer the expected type to infer generic
  types, functions, and methods (SDK
  issue [27586](https://github.com/dart-lang/sdk/issues/27586)).

  ```dart
  main() {
    List<Object> foo = /*infers: <Object>*/['hello', 'world'];
    var bar = /*infers: <String>*/['hello', 'world'];
  }
  ```

* Strong mode inference error messages are improved
  (SDK issue [29108](https://github.com/dart-lang/sdk/issues/29108)).

  ```dart
  import 'dart:math';
  test(Iterable/* fix is to add <num> here */ values) {
    num n = values.fold(values.first as num, max);
  }
  ```
  Now produces the error on the generic function "max":
  ```
  Couldn't infer type parameter 'T'.

  Tried to infer 'dynamic' for 'T' which doesn't work:
    Function type declared as '<T extends num>(T, T) → T'
                  used where  '(num, dynamic) → num' is required.

  Consider passing explicit type argument(s) to the generic.
  ```

* Strong mode supports overriding fields, `@virtual` is no longer required
    (SDK issue [28120](https://github.com/dart-lang/sdk/issues/28120)).

    ```dart
    class C {
      int x = 42;
    }
    class D extends C {
      get x {
        print("x got called");
        return super.x;
      }
    }
    main() {
      print(new D().x);
    }
    ```

* Strong mode down cast composite warnings are no longer issued by default.
  (SDK issue [28588](https://github.com/dart-lang/sdk/issues/28588)).

```dart
void test() {
  List untyped = [];
  List<int> typed = untyped; // No down cast composite warning
}
```

To opt back into the warnings, add the following to
the
[.analysis_options](https://www.dartlang.org/guides/language/analysis-options)
file for your project.

```
analyzer:
  errors:
    strong_mode_down_cast_composite: warning
```


### Core library changes

* `dart:core`
  * Added `Uri.isScheme` function to check the scheme of a URI.
    Example: `uri.isScheme("http")`. Ignores case when comparing.
  * Make `UriData.parse` validate its input better.
    If the data is base-64 encoded, the data is normalized wrt.
    alphabet and padding, and it contains invalid base-64 data,
    parsing fails. Also normalizes non-base-64 data.
* `dart:io`
  * Added functions `File.lastAccessed`, `File.lastAccessedSync`,
    `File.setLastModified`, `File.setLastModifiedSync`, `File.setLastAccessed`,
    and `File.setLastAccessedSync`.
  * Added `{Stdin,Stdout}.supportsAnsiEscapes`.

### Dart VM

* Calls to `print()` and `Stdout.write*()` now correctly print unicode
  characters to the console on Windows. Calls to `Stdout.add*()` behave as
  before.

### Tool changes

* Analysis
  * `dartanalyzer` now follows the same rules as the analysis server to find
    an analysis options file, stopping when an analysis options file is found:
    * Search up the directory hierarchy looking for an analysis options file.
    * If analyzing a project referencing the [Flutter](https://flutter.io/)
      package, then use the
      [default Flutter analysis options](https://github.com/flutter/flutter/blob/master/packages/flutter/lib/analysis_options_user.yaml)
      found in `package:flutter`.
    * If in a Bazel workspace, then use the analysis options in
      `package:dart.analysis_options/default.yaml` if it exists.
    * Use the default analysis options rules.
  * In addition, specific to `dartanalyzer`:
    * an analysis options file can be specified on the command line via
      `--options` and that file will be used instead of searching for an
      analysis options file.
    * any analysis option specified on the command line
      (e.g. `--strong` or `--no-strong`) takes precedence over any corresponding
      value specified in the analysis options file.

* Dartium, dart2js, and DDC

  * Imports to `dart:io` are allowed, but the imported library is not supported
    and will likely fail on most APIs at runtime. This change was made as a
    stopgap measure to make it easier to write libraries that share code between
    platforms (like package `http`). This might change again when configuration
    specific imports are supported.

* Pub
  * Now sends telemetry data to `pub.dartlang.org` to allow better understanding
    of why a particular package is being accessed.
  * `pub publish`
    * Warns if a package imports a package that's not a dependency from within
      `lib/` or `bin/`, or a package that's not a dev dependency from within
      `benchmark/`, `example/`, `test/` or `tool/`.
    * No longer produces "UID too large" errors on OS X. All packages are now
      uploaded with the user and group names set to "pub".
    * No longer fails with a stack overflow when uploading a package that uses
      Git submodules.
  * `pub get` and `pub upgrade`
    * Produce more informative error messages if they're run directly in a
      package that uses Flutter.
    * Properly unlock SDK and path dependencies if they have a new version
      that's also valid according to the user's pubspec.

* dartfmt
  * Support new generic function typedef syntax.
  * Make the precedence of cascades more visible.
  * Fix a couple of places where spurious newlines were inserted.
  * Correctly report unchanged formatting when reading from stdin.
  * Ensure space between `-` and `--`. Code that does this is pathological, but
    it technically meant dartfmt could change the semantics of the code.
  * Preserve a blank line between enum cases.
  * Other small formatting tweaks.


## 1.22.1 - 2017-02-22

Patch release, resolves two issues:
* Dart VM crash: [Issue 28072](https://github.com/dart-lang/sdk/issues/28757)

* Dart VM bug combining types, await, and deferred loading: [Issue 28678](https://github.com/dart-lang/sdk/issues/28678)


## 1.22.0 - 2017-02-14

### Language

  * Breaking change:
    ['Generalized tear-offs'](https://github.com/gbracha/generalizedTearOffs/blob/master/proposal.md)
    are no longer supported, and will cause errors. We updated the language spec
    and added warnings in 1.21, and are now taking the last step to fully
    de-support them. They were previously only supported in the VM, and there
    are almost no known uses of them in the wild.

  * The `assert()` statement has been expanded to support an optional second
    `message` argument
    (SDK issue [27342](https://github.com/dart-lang/sdk/issues/27342)).

    The message is displayed if the assert fails. It can be any object, and it
    is accessible as `AssertionError.message`. It can be used to provide more
    user friendly exception outputs. As an example, the following assert:

    ```dart
    assert(configFile != null, "Tool config missing. Please see https://goo.gl/k8iAi for details.");
    ```

    would produce the following exception output:

    ```
    Unhandled exception:
    'file:///Users/mit/tmp/tool/bin/main.dart': Failed assertion: line 9 pos 10:
    'configFile != null': Tool config missing. Please see https://goo.gl/k8iAi for details.
    #0      _AssertionError._doThrowNew (dart:core-patch/errors_patch.dart:33)
    #1      _AssertionError._throwNew (dart:core-patch/errors_patch.dart:29)
    #2      main (file:///Users/mit/tmp/tool/bin/main.dart:9:10)
    ```

  * The `Null` type has been moved to the bottom of the type hierarchy. As such,
    it is considered a subtype of every other type. The `null` *literal* was
    always treated as a bottom type. Now the named class `Null` is too:

    ```dart
    const empty = <Null>[];

    String concatenate(List<String> parts) => parts.join();
    int sum(List<int> numbers) => numbers.fold(0, (sum, n) => sum + n);

    concatenate(empty); // OK.
    sum(empty); // OK.
    ```

  * Introduce `covariant` modifier on parameters. It indicates that the
    parameter (and the corresponding parameter in any method that overrides it)
    has looser override rules. In strong mode, these require a runtime type
    check to maintain soundness, but enable an architectural pattern that is
    useful in some code.

    It lets you specialize a family of classes together, like so:

    ```dart
    abstract class Predator {
      void chaseAndEat(covariant Prey p);
    }

    abstract class Prey {}

    class Mouse extends Prey {}

    class Seal extends Prey {}

    class Cat extends Predator {
      void chaseAndEat(Mouse m) => ...
    }

    class Orca extends Predator {
      void chaseAndEat(Seal s) => ...
    }
    ```

    This isn't statically safe, because you could do:

    ```dart
    Predator predator = new Cat(); // Upcast.
    predator.chaseAndEat(new Seal()); // Cats can't eat seals!
    ```

    To preserve soundness in strong mode, in the body of a method that uses a
    covariant override (here, `Cat.chaseAndEat()`), the compiler automatically
    inserts a check that the parameter is of the expected type. So the compiler
    gives you something like:

    ```dart
    class Cat extends Predator {
      void chaseAndEat(o) {
        var m = o as Mouse;
        ...
      }
    }
    ```

    Spec mode allows this unsound behavior on all parameters, even though users
    rarely rely on it. Strong mode disallowed it initially. Now, strong mode
    lets you opt into this behavior in the places where you do want it by using
    this modifier. Outside of strong mode, the modifier is ignored.

  * Change instantiate-to-bounds rules for generic type parameters when running
    in strong mode. If you leave off the type parameters from a generic type, we
    need to decide what to fill them in with.  Dart 1.0 says just use `dynamic`,
    but that isn't sound:

    ```dart
    class Abser<T extends num> {
       void absThis(T n) { n.abs(); }
    }

    var a = new Abser(); // Abser<dynamic>.
    a.absThis("not a num");
    ```

    We want the body of `absThis()` to be able to safely assume `n` is at
    least a `num` -- that's why there's a constraint on T, after all. Implicitly
    using `dynamic` as the type parameter in this example breaks that.

    Instead, strong mode uses the bound. In the above example, it fills it in
    with `num`, and then the second line where a string is passed becomes a
    static error.

    However, there are some cases where it is hard to figure out what that
    default bound should be:

    ```dart
    class RuhRoh<T extends Comparable<T>> {}
    ```

    Strong mode's initial behavior sometimes produced surprising, unintended
    results. For 1.22, we take a simpler approach and then report an error if
    a good default type argument can't be found.

### Core libraries

  * Define `FutureOr<T>` for code that works with either a future or an
    immediate value of some type. For example, say you do a lot of text
    manipulation, and you want a handy function to chain a bunch of them:

    ```dart
    typedef String StringSwizzler(String input);

    String swizzle(String input, List<StringSwizzler> swizzlers) {
      var result = input;
      for (var swizzler in swizzlers) {
        result = swizzler(result);
      }

      return result;
    }
    ```

    This works fine:

    ```dart
    main() {
      var result = swizzle("input", [
        (s) => s.toUpperCase(),
        (s) => () => s * 2)
      ]);
      print(result); // "INPUTINPUT".
    }
    ```

    Later, you realize you'd also like to support swizzlers that are
    asynchronous (maybe they look up synonyms for words online). You could make
    your API strictly asynchronous, but then users of simple synchronous
    swizzlers have to manually wrap the return value in a `Future.value()`.
    Ideally, your `swizzle()` function would be "polymorphic over asynchrony".
    It would allow both synchronous and asynchronous swizzlers. Because `await`
    accepts immediate values, it is easy to implement this dynamically:

    ```dart
    Future<String> swizzle(String input, List<StringSwizzler> swizzlers) async {
      var result = input;
      for (var swizzler in swizzlers) {
        result = await swizzler(result);
      }

      return result;
    }

    main() async {
      var result = swizzle("input", [
        (s) => s.toUpperCase(),
        (s) => new Future.delayed(new Duration(milliseconds: 40), () => s * 2)
      ]);
      print(await result);
    }
    ```

    What should the declared return type on StringSwizzler be? In the past, you
    had to use `dynamic` or `Object`, but that doesn't tell the user much. Now,
    you can do:

    ```dart
    typedef FutureOr<String> StringSwizzler(String input);
    ```

    Like the name implies, `FutureOr<String>` is a union type. It can be a
    `String` or a `Future<String>`, but not anything else. In this case, that's
    not super useful beyond just stating a more precise type for readers of the
    code. It does give you a little better error checking in code that uses the
    result of that.

    `FutureOr<T>` becomes really important in *generic* methods like
    `Future.then()`. In those cases, having the type system understand this
    magical union type helps type inference figure out the type argument of
    `then()` based on the closure you pass it.

    Previously, strong mode had hard-coded rules for handling `Future.then()`
    specifically. `FutureOr<T>` exposes that functionality so third-party APIs
    can take advantage of it too.

### Tool changes

* Dart2Js

  * Remove support for (long-time deprecated) mixin typedefs.

* Pub

  * Avoid using a barback asset server for executables unless they actually use
    transformers. This makes precompilation substantially faster, produces
    better error messages when precompilation fails, and allows
    globally-activated executables to consistently use the
    `Isolate.resolvePackageUri()` API.

  * On Linux systems, always ignore packages' original file owners and
    permissions when extracting those packages. This was already the default
    under most circumstances.

  * Properly close the standard input stream of child processes started using
    `pub run`.

  * Handle parse errors from the package cache more gracefully. A package whose
    pubspec can't be parsed will now be ignored by `pub get --offline` and
    deleted by `pub cache repair`.

  * Make `pub run` run executables in spawned isolates. This lets them handle
    signals and use standard IO reliably.

  * Fix source-maps produced by dart2js when running in `pub serve`: URL
    references to assets from packages match the location where `pub serve`
    serves them (`packages/package_name/` instead of
    `../packages/package_name/`).

### Infrastructure changes

  * The SDK now uses GN rather than gyp to generate its build files, which will
    now be exclusively ninja flavored. Documentation can be found on our
    [wiki](https://github.com/dart-lang/sdk/wiki/Building-with-GN). Also see the
    help message of `tools/gn.py`. This change is in response to the deprecation
    of gyp. Build file generation with gyp will continue to be available in this
    release by setting the environment variable `DART_USE_GYP` before running
    `gclient sync` or `gclient runhooks`, but this will be removed in a future
    release.

## 1.21.1 - 2017-01-13

Patch release, resolves one issue:

* Dart VM: Snapshots of generic functions fail. [Issue 28072](https://github.com/dart-lang/sdk/issues/28072)

## 1.21.0 - 2016-12-07

### Language

* Support generic method syntax. Type arguments are not available at
  runtime. For details, check the
  [informal specification](https://gist.github.com/eernstg/4353d7b4f669745bed3a5423e04a453c).
* Support access to initializing formals, e.g., the use of `x` to initialize
 `y` in `class C { var x, y; C(this.x): y = x; }`.
  Please check the
  [informal specification](https://gist.github.com/eernstg/cff159be9e34d5ea295d8c24b1a3e594)
  for details.
* Don't warn about switch case fallthrough if the case ends in a `rethrow`
  statement.  (SDK issue
  [27650](https://github.com/dart-lang/sdk/issues/27650))
* Also don't warn if the entire switch case is wrapped in braces - as long as
  the block ends with a `break`, `continue`, `rethrow`, `return` or `throw`.
* Allow `=` as well as `:` as separator for named parameter default values.

  ```dart
  enableFlags({bool hidden: false}) { … }
  ```

  can now be replaced by

  ```dart
  enableFlags({bool hidden = false}) { … }
  ```

  (SDK issue [27559](https://github.com/dart-lang/sdk/issues/27559))

### Core library changes

* `dart:core`: `Set.difference` now takes a `Set<Object>` as argument.  (SDK
  issue [27573](https://github.com/dart-lang/sdk/issues/27573))

* `dart:developer`

  * Added `Service` class.
    * Allows inspecting and controlling the VM service protocol HTTP server.
    * Provides an API to access the ID of an `Isolate`.

### Tool changes

* Dart Dev Compiler

  * Support calls to `loadLibrary()` on deferred libraries. Deferred libraries
    are still loaded eagerly. (SDK issue
    [27343](https://github.com/dart-lang/sdk/issues/27343))

## 1.20.1 - 2016-10-13

Patch release, resolves one issue:

* Dartium: Fixes a bug that caused crashes.  No issue filed.

### Strong Mode

* It is no longer a warning when casting from dynamic to a composite type
    (SDK issue [27766](https://github.com/dart-lang/sdk/issues/27766)).

    ```dart
    main() {
      dynamic obj = <int>[1, 2, 3];
      // This is now allowed without a warning.
      List<int> list = obj;
    }
    ```

## 1.20.0 - 2016-10-11

### Dart VM

* We have improved the way that the VM locates the native code library for a
  native extension (e.g. `dart-ext:` import). We have updated this
  [article on native extensions](https://www.dartlang.org/articles/dart-vm/native-extensions)
  to reflect the VM's improved behavior.

* Linux builds of the VM will now use the `tcmalloc` library for memory
  allocation. This has the advantages of better debugging and profiling support
  and faster small allocations, with the cost of slightly larger initial memory
  footprint, and slightly slower large allocations.

* We have improved the way the VM searches for trusted root certificates for
  secure socket connections on Linux. First, the VM will look for trusted root
  certificates in standard locations on the file system
  (`/etc/pki/tls/certs/ca-bundle.crt` followed by `/etc/ssl/certs`), and only if
  these do not exist will it fall back on the builtin trusted root certificates.
  This behavior can be overridden on Linux with the new flags
  `--root-certs-file` and `--root-certs-cache`. The former is the path to a file
  containing the trusted root certificates, and the latter is the path to a
  directory containing root certificate files hashed using `c_rehash`.

* The VM now throws a catchable `Error` when method compilation fails. This
  allows easier debugging of syntax errors, especially when testing.  (SDK issue
  [23684](https://github.com/dart-lang/sdk/issues/23684))

### Core library changes

* `dart:core`: Remove deprecated `Resource` class.
  Use the class in `package:resource` instead.
* `dart:async`
  * `Future.wait` now catches synchronous errors and returns them in the
    returned Future.  (SDK issue
    [27249](https://github.com/dart-lang/sdk/issues/27249))
  * More aggressively returns a `Future` on `Stream.cancel` operations.
    Discourages to return `null` from `cancel`.  (SDK issue
    [26777](https://github.com/dart-lang/sdk/issues/26777))
  * Fixes a few bugs where the cancel future wasn't passed through
    transformations.
* `dart:io`
  * Added `WebSocket.addUtf8Text` to allow sending a pre-encoded text message
    without a round-trip UTF-8 conversion.  (SDK issue
    [27129](https://github.com/dart-lang/sdk/issues/27129))

### Strong Mode

* Breaking change - it is an error if a generic type parameter cannot be
    inferred (SDK issue [26992](https://github.com/dart-lang/sdk/issues/26992)).

    ```dart
    class Cup<T> {
      Cup(T t);
    }
    main() {
      // Error because:
      // - if we choose Cup<num> it is not assignable to `cOfInt`,
      // - if we choose Cup<int> then `n` is not assignable to int.
      num n;
      C<int> cOfInt = new C(n);
    }
    ```

* New feature - use `@checked` to override a method and tighten a parameter
    type (SDK issue [25578](https://github.com/dart-lang/sdk/issues/25578)).

    ```dart
    import 'package:meta/meta.dart' show checked;
    class View {
      addChild(View v) {}
    }
    class MyView extends View {
      // this override is legal, it will check at runtime if we actually
      // got a MyView.
      addChild(@checked MyView v) {}
    }
    main() {
      dynamic mv = new MyView();
      mv.addChild(new View()); // runtime error
    }
    ```

* New feature - use `@virtual` to allow field overrides in strong mode
    (SDK issue [27384](https://github.com/dart-lang/sdk/issues/27384)).

    ```dart
    import 'package:meta/meta.dart' show virtual;
    class Base {
      @virtual int x;
    }
    class Derived extends Base {
      int x;

      // Expose the hidden storage slot:
      int get superX => super.x;
      set superX(int v) { super.x = v; }
    }
    ```

* Breaking change - infer list and map literals from the context type as well as
    their values, consistent with generic methods and instance creation
    (SDK issue [27151](https://github.com/dart-lang/sdk/issues/27151)).

    ```dart
    import 'dart:async';
    main() async {
      var b = new Future<B>.value(new B());
      var c = new Future<C>.value(new C());
      var/*infer List<Future<A>>*/ list = [b, c];
      var/*infer List<A>*/ result = await Future.wait(list);
    }
    class A {}
    class B extends A {}
    class C extends A {}
    ```

### Tool changes

* `dartfmt` - upgraded to v0.2.10
    * Don't crash on annotations before parameters with trailing commas.
    * Always split enum declarations if they end in a trailing comma.
    * Add `--set-exit-if-changed` to set the exit code on a change.

* Pub
  * Pub no longer generates a `packages/` directory by default.  Instead, it
    generates a `.packages` file, called a package spec. To generate
    a `packages/` directory in addition to the package spec, use the
    `--packages-dir` flag with `pub get`, `pub upgrade`, and `pub downgrade`.
    See the [Good-bye
    symlinks](http://news.dartlang.org/2016/10/good-bye-symlinks.html) article
    for details.

## 1.19.1 - 2016-09-08

Patch release, resolves one issue:

* Dartdoc:  Fixes a bug that prevented generation of docs.
  (Dartdoc issue [1233](https://github.com/dart-lang/dartdoc/issues/1233))

## 1.19.0 - 2016-08-26

### Language changes

* The language now allows a trailing comma after the last argument of a call and
 the last parameter of a function declaration. This can make long argument or
 parameter lists easier to maintain, as commas can be left as-is when
 reordering lines. For details, see SDK issue
 [26644](https://github.com/dart-lang/sdk/issues/26644).

### Tool Changes

* `dartfmt` - upgraded to v0.2.9+1
  * Support trailing commas in argument and parameter lists.
  * Gracefully handle read-only files.
  * About a dozen other bug fixes.

* Pub
  * Added a `--no-packages-dir` flag to `pub get`, `pub upgrade`, and `pub
    downgrade`. When this flag is passed, pub will not generate a `packages/`
    directory, and will remove that directory and any symlinks to it if they
    exist. Note that this replaces the unsupported `--no-package-symlinks` flag.

  * Added the ability for packages to declare a constraint on the [Flutter][]
    SDK:

    ```yaml
    environment:
      flutter: ^0.1.2
      sdk: >=1.19.0 <2.0.0
    ```

    A Flutter constraint will only be satisfiable when pub is running in the
    context of the `flutter` executable, and when the Flutter SDK version
    matches the constraint.

  * Added `sdk` as a new package source that fetches packages from a hard-coded
    SDK. Currently only the `flutter` SDK is supported:

    ```yaml
    dependencies:
      flutter_driver:
        sdk: flutter
        version: ^0.0.1
    ```

    A Flutter `sdk` dependency will only be satisfiable when pub is running in
    the context of the `flutter` executable, and when the Flutter SDK contains a
    package with the given name whose version matches the constraint.

  * `tar` files on Linux are now created with `0` as the user and group IDs.
    This fixes a crash when publishing packages while using Active Directory.

  * Fixed a bug where packages from a hosted HTTP URL were considered the same
    as packages from an otherwise-identical HTTPS URL.

  * Fixed timer formatting for timers that lasted longer than a minute.

  * Eliminate some false negatives when determining whether global executables
    are on the user's executable path.

* `dart2js`
  * `dart2dart` (aka `dart2js --output-type=dart`) has been removed (this was deprecated in Dart 1.11).

[Flutter]: https://flutter.io/

### Dart VM

*   The dependency on BoringSSL has been rolled forward. Going forward, builds
    of the Dart VM including secure sockets will require a compiler with C++11
    support. For details, see the
    [Building wiki page](https://github.com/dart-lang/sdk/wiki/Building).

### Strong Mode

*   New feature - an option to disable implicit casts
    (SDK issue [26583](https://github.com/dart-lang/sdk/issues/26583)),
    see the [documentation](https://github.com/dart-lang/dev_compiler/blob/master/doc/STATIC_SAFETY.md#disable-implicit-casts)
    for usage instructions and examples.

*   New feature - an option to disable implicit dynamic
    (SDK issue [25573](https://github.com/dart-lang/sdk/issues/25573)),
    see the [documentation](https://github.com/dart-lang/dev_compiler/blob/master/doc/STATIC_SAFETY.md#disable-implicit-dynamic)
    for usage instructions and examples.

*   Breaking change - infer generic type arguments from the
    constructor invocation arguments
    (SDK issue [25220](https://github.com/dart-lang/sdk/issues/25220)).

    ```dart
    var map = new Map<String, String>();

    // infer: Map<String, String>
    var otherMap = new Map.from(map);
    ```

*   Breaking change - infer local function return type
    (SDK issue [26414](https://github.com/dart-lang/sdk/issues/26414)).

    ```dart
    void main() {
      // infer: return type is int
      f() { return 40; }
      int y = f() + 2; // type checks
      print(y);
    }
    ```

*   Breaking change - allow type promotion from a generic type parameter
    (SDK issue [26414](https://github.com/dart-lang/sdk/issues/26965)).

    ```dart
    void fn/*<T>*/(/*=T*/ object) {
      if (object is String) {
        // Treat `object` as `String` inside this block.
        // But it will require a cast to pass it to something that expects `T`.
        print(object.substring(1));
      }
    }
    ```

* Breaking change - smarter inference for Future.then
    (SDK issue [25944](https://github.com/dart-lang/sdk/issues/25944)).
    Previous workarounds that use async/await or `.then/*<Future<SomeType>>*/`
    should no longer be necessary.

    ```dart
    // This will now infer correctly.
    Future<List<int>> t2 = f.then((_) => [3]);
    // This infers too.
    Future<int> t2 = f.then((_) => new Future.value(42));
    ```

* Breaking change - smarter inference for async functions
    (SDK issue [25322](https://github.com/dart-lang/sdk/issues/25322)).

    ```dart
    void test() async {
      List<int> x = await [4]; // was previously inferred
      List<int> y = await new Future.value([4]); // now inferred too
    }
    ```

* Breaking change - sideways casts are no longer allowed
    (SDK issue [26120](https://github.com/dart-lang/sdk/issues/26120)).

## 1.18.1 - 2016-08-02

Patch release, resolves two issues and improves performance:

* Debugger: Fixes a bug that crashes the VM
(SDK issue [26941](https://github.com/dart-lang/sdk/issues/26941))

* VM: Fixes an optimizer bug involving closures, try, and await
(SDK issue [26948](https://github.com/dart-lang/sdk/issues/26948))

* Dart2js: Speeds up generated code on Firefox
(https://codereview.chromium.org/2180533002)

## 1.18.0 - 2016-07-27

### Core library changes

* `dart:core`
  * Improved performance when parsing some common URIs.
  * Fixed bug in `Uri.resolve` (SDK issue [26804](https://github.com/dart-lang/sdk/issues/26804)).
* `dart:io`
  * Adds file locking modes `FileLock.BLOCKING_SHARED` and
    `FileLock.BLOCKING_EXCLUSIVE`.

## 1.17.1 - 2016-06-10

Patch release, resolves two issues:

* VM: Fixes a bug that caused crashes in async functions.
(SDK issue [26668](https://github.com/dart-lang/sdk/issues/26668))

* VM: Fixes a bug that caused garbage collection of reachable weak properties.
(https://codereview.chromium.org/2041413005)

## 1.17.0 - 2016-06-08

### Core library changes
* `dart:convert`
  * Deprecate `ChunkedConverter` which was erroneously added in 1.16.

* `dart:core`
  * `Uri.replace` supports iterables as values for the query parameters.
  * `Uri.parseIPv6Address` returns a `Uint8List`.

* `dart:io`
  * Added `NetworkInterface.listSupported`, which is `true` when
    `NetworkInterface.list` is supported, and `false` otherwise. Currently,
    `NetworkInterface.list` is not supported on Android.

### Tool Changes

* Pub
  * TAR files created while publishing a package on Mac OS and Linux now use a
    more portable format.

  * Errors caused by invalid arguments now print the full usage information for
    the command.

  * SDK constraints for dependency overrides are no longer considered when
    determining the total SDK constraint for a lockfile.

  * A bug has been fixed in which a lockfile was considered up-to-date when it
    actually wasn't.

  * A bug has been fixed in which `pub get --offline` would crash when a
    prerelease version was selected.

* Dartium and content shell
  * Debugging Dart code inside iframes improved, was broken.

## 1.16.1 - 2016-05-24

Patch release, resolves one issue:

* VM: Fixes a bug that caused intermittent hangs on Windows.
(SDK issue [26400](https://github.com/dart-lang/sdk/issues/26400))

## 1.16.0 - 2016-04-26

### Core library changes

* `dart:convert`
  * Added `BASE64URL` codec and corresponding `Base64Codec.urlSafe` constructor.

  * Introduce `ChunkedConverter` and deprecate chunked methods on `Converter`.

* `dart:html`

  There have been a number of **BREAKING** changes to align APIs with recent
  changes in Chrome. These include:

  * Chrome's `ShadowRoot` interface no longer has the methods `getElementById`,
    `getElementsByClassName`, and `getElementsByTagName`, e.g.,

    ```dart
    elem.shadowRoot.getElementsByClassName('clazz')
    ```

    should become:

    ```dart
    elem.shadowRoot.querySelectorAll('.clazz')
    ```

  * The `clipboardData` property has been removed from `KeyEvent`
    and `Event`. It has been moved to the new `ClipboardEvent` class, which is
    now used by `copy`, `cut`, and `paste` events.

  * The `layer` property has been removed from `KeyEvent` and
    `UIEvent`. It has been moved to `MouseEvent`.

  * The `Point get page` property has been removed from `UIEvent`.
    It still exists on `MouseEvent` and `Touch`.

  There have also been a number of other additions and removals to `dart:html`,
  `dart:indexed_db`, `dart:svg`, `dart:web_audio`, and `dart:web_gl` that
  correspond to changes to Chrome APIs between v39 and v45. Many of the breaking
  changes represent APIs that would have caused runtime exceptions when compiled
  to Javascript and run on recent Chrome releases.

* `dart:io`
  * Added `SecurityContext.alpnSupported`, which is true if a platform
    supports ALPN, and false otherwise.

### JavaScript interop

For performance reasons, a potentially **BREAKING** change was added for
libraries that use JS interop.
Any Dart file that uses `@JS` annotations on declarations (top-level functions,
classes or class members) to interop with JavaScript code will require that the
file have the annotation `@JS()` on a library directive.

```dart
@JS()
library my_library;
```

The analyzer will enforce this by generating the error:

The `@JS()` annotation can only be used if it is also declared on the library
directive.

If part file uses the `@JS()` annotation, the library that uses the part should
have the `@JS()` annotation e.g.,

```dart
// library_1.dart
@JS()
library library_1;

import 'package:js/js.dart';

part 'part_1.dart';
```

```dart
// part_1.dart
part of library_1;

@JS("frameworkStabilizers")
external List<FrameworkStabilizer> get frameworkStabilizers;
```

If your library already has a JS module e.g.,

```dart
@JS('array.utils')
library my_library;
```

Then your library will work without any additional changes.

### Analyzer

*   Static checking of `for in` statements. These will now produce static
    warnings:

    ```dart
    // Not Iterable.
    for (var i in 1234) { ... }

    // String cannot be assigned to int.
    for (int n in <String>["a", "b"]) { ... }
    ```

### Tool Changes

* Pub
  * `pub serve` now provides caching headers that should improve the performance
    of requesting large files multiple times.

  * Both `pub get` and `pub upgrade` now have a `--no-precompile` flag that
    disables precompilation of executables and transformed dependencies.

  * `pub publish` now resolves symlinks when publishing from a Git repository.
    This matches the behavior it always had when publishing a package that
    wasn't in a Git repository.

* Dart Dev Compiler
  * The **experimental** `dartdevc` executable has been added to the SDK.

  * It will help early adopters validate the implementation and provide
    feedback. `dartdevc` **is not** yet ready for production usage.

  * Read more about the Dart Dev Compiler [here][dartdevc].

[dartdevc]: https://github.com/dart-lang/dev_compiler

## 1.15.0 - 2016-03-09

### Core library changes

* `dart:async`
  * Made `StreamView` class a `const` class.

* `dart:core`
  * Added `Uri.queryParametersAll` to handle multiple query parameters with
    the same name.

* `dart:io`
  * Added `SecurityContext.usePrivateKeyBytes`,
    `SecurityContext.useCertificateChainBytes`,
    `SecurityContext.setTrustedCertificatesBytes`, and
    `SecurityContext.setClientAuthoritiesBytes`.
  * **Breaking** The named `directory` argument of
    `SecurityContext.setTrustedCertificates` has been removed.
  * Added support to `SecurityContext` for PKCS12 certificate and key
    containers.
  * All calls in `SecurityContext` that accept certificate data now accept an
    optional named parameter `password`, similar to
    `SecurityContext.usePrivateKeyBytes`, for use as the password for PKCS12
    data.

### Tool changes

* Dartium and content shell
  * The Chrome-based tools that ship as part of the Dart SDK - Dartium and
    content shell - are now based on Chrome version 45 (instead of Chrome 39).
  * Dart browser libraries (`dart:html`, `dart:svg`, etc) *have not* been
    updated.
    * These are still based on Chrome 39.
    * These APIs will be updated in a future release.
  * Note that there are experimental APIs which have changed in the underlying
    browser, and will not work with the older libraries.
    For example, `Element.animate`.

* `dartfmt` - upgraded to v0.2.4
  * Better handling for long collections with comments.
  * Always put member metadata annotations on their own line.
  * Indent functions in named argument lists with non-functions.
  * Force the parameter list to split if a split occurs inside a function-typed
    parameter.
  * Don't force a split for before a single named argument if the argument
    itself splits.

### Service protocol changes

* Fixed a documentation bug where the field `extensionRPCs` in `Isolate`
  was not marked optional.

### Experimental language features
  * Added support for [configuration-specific imports](https://github.com/munificent/dep-interface-libraries/blob/master/Proposal.md).
    On the VM and `dart2js`, they can be enabled with `--conditional-directives`.

    The analyzer requires additional configuration:
    ```yaml
    analyzer:
      language:
        enableConditionalDirectives: true
    ```

    Read about [configuring the analyzer] for more details.

[configuring the analyzer]: https://github.com/dart-lang/sdk/tree/master/pkg/analyzer#configuring-the-analyzer

## 1.14.2 - 2016-02-10

Patch release, resolves three issues:

* VM: Fixed a code generation bug on x64.
  (SDK commit [834b3f02](https://github.com/dart-lang/sdk/commit/834b3f02b6ab740a213fd808e6c6f3269bed80e5))

* `dart:io`: Fixed EOF detection when reading some special device files.
  (SDK issue [25596](https://github.com/dart-lang/sdk/issues/25596))

* Pub: Fixed an error using hosted dependencies in SDK version 1.14.
  (Pub issue [1386](https://github.com/dart-lang/pub/issues/1386))

## 1.14.1 - 2016-02-04

Patch release, resolves one issue:

* Debugger: Fixes a VM crash when a debugger attempts to set a break point
during isolate initialization.
(SDK issue [25618](https://github.com/dart-lang/sdk/issues/25618))

## 1.14.0 - 2016-01-28

### Core library changes
* `dart:async`
  * Added `Future.any` static method.
  * Added `Stream.fromFutures` constructor.

* `dart:convert`
  * `Base64Decoder.convert` now takes optional `start` and `end` parameters.

* `dart:core`
  * Added `current` getter to `StackTrace` class.
  * `Uri` class added support for data URIs
      * Added two new constructors: `dataFromBytes` and `dataFromString`.
      * Added a `data` getter for `data:` URIs with a new `UriData` class for
      the return type.
  * Added `growable` parameter to `List.filled` constructor.
  * Added microsecond support to `DateTime`: `DateTime.microsecond`,
    `DateTime.microsecondsSinceEpoch`, and
    `new DateTime.fromMicrosecondsSinceEpoch`.

* `dart:math`
  * `Random` added a `secure` constructor returning a cryptographically secure
    random generator which reads from the entropy source provided by the
    embedder for every generated random value.

* `dart:io`
  * `Platform` added a static `isIOS` getter and `Platform.operatingSystem` may
    now return `ios`.
  * `Platform` added a static `packageConfig` getter.
  * Added support for WebSocket compression as standardized in RFC 7692.
  * Compression is enabled by default for all WebSocket connections.
      * The optionally named parameter `compression` on the methods
      `WebSocket.connect`, `WebSocket.fromUpgradedSocket`, and
      `WebSocketTransformer.upgrade` and  the `WebSocketTransformer`
      constructor can be used to modify or disable compression using the new
      `CompressionOptions` class.

* `dart:isolate`
  * Added **_experimental_** support for [Package Resolution Configuration].
    * Added `packageConfig` and `packageRoot` instance getters to `Isolate`.
    * Added a `resolvePackageUri` method to `Isolate`.
    * Added named arguments `packageConfig` and `automaticPackageResolution` to
    the `Isolate.spawnUri` constructor.

[Package Resolution Configuration]: https://github.com/dart-lang/dart_enhancement_proposals/blob/master/Accepted/0005%20-%20Package%20Specification/DEP-pkgspec.md

### Tool changes

* `dartfmt`

  * Better line splitting in a variety of cases.

  * Other optimizations and bug fixes.

* Pub

  * **Breaking:** Pub now eagerly emits an error when a pubspec's "name" field
    is not a valid Dart identifier. Since packages with non-identifier names
    were never allowed to be published, and some of them already caused crashes
    when being written to a `.packages` file, this is unlikely to break many
    people in practice.

  * **Breaking:** Support for `barback` versions prior to 0.15.0 (released July
    1)    has been dropped. Pub will no longer install these older barback
    versions.

  * `pub serve` now GZIPs the assets it serves to make load times more similar
    to real-world use-cases.

  * `pub deps` now supports a `--no-dev` flag, which causes it to emit the
    dependency tree as it would be if no `dev_dependencies` were in use. This
    makes it easier to see your package's dependency footprint as your users
    will experience it.

  * `pub global run` now detects when a global executable's SDK constraint is no
    longer met and errors out, rather than trying to run the executable anyway.

  * Pub commands that check whether the lockfile is up-to-date (`pub run`, `pub
    deps`, `pub serve`, and `pub build`) now do additional verification. They
    ensure that any path dependencies' pubspecs haven't been changed, and they
    ensure that the current SDK version is compatible with all dependencies.

  * Fixed a crashing bug when using `pub global run` on a global script that
    didn't exist.

  * Fixed a crashing bug when a pubspec contains a dependency without a source
    declared.

## 1.13.2 - 2016-01-06

Patch release, resolves one issue:

* dart2js: Stack traces are not captured correctly (SDK issue [25235]
(https://github.com/dart-lang/sdk/issues/25235))

## 1.13.1 - 2015-12-17

Patch release, resolves three issues:

* VM type propagation fix: Resolves a potential crash in the Dart VM (SDK commit
 [dff13be]
(https://github.com/dart-lang/sdk/commit/dff13bef8de104d33b04820136da2d80f3c835d7))

* dart2js crash fix: Resolves a crash in pkg/js and dart2js (SDK issue [24974]
(https://github.com/dart-lang/sdk/issues/24974))

* Pub get crash on ARM: Fixes a crash triggered when running 'pub get' on ARM
 processors such as those on a Raspberry Pi (SDK issue [24855]
(https://github.com/dart-lang/sdk/issues/24855))

## 1.13.0 - 2015-11-18

### Core library changes
* `dart:async`
  * `StreamController` added getters for `onListen`, `onPause`, and `onResume`
    with the corresponding new `typedef void ControllerCallback()`.
  * `StreamController` added a getter for `onCancel` with the corresponding
    new `typedef ControllerCancelCallback()`;
  * `StreamTransformer` instances created with `fromHandlers` with no
    `handleError` callback now forward stack traces along with errors to the
    resulting streams.

* `dart:convert`
  * Added support for Base-64 encoding and decoding.
    * Added new classes `Base64Codec`, `Base64Encoder`, and `Base64Decoder`.
    * Added new top-level `const Base64Codec BASE64`.

* `dart:core`
  * `Uri` added `removeFragment` method.
  * `String.allMatches` (implementing `Pattern.allMatches`) is now lazy,
    as all `allMatches` implementations are intended to be.
  * `Resource` is deprecated, and will be removed in a future release.

* `dart:developer`
  * Added `Timeline` class for interacting with Observatory's timeline feature.
  * Added `ServiceExtensionHandler`, `ServiceExtensionResponse`, and `registerExtension` which enable developers to provide their own VM service protocol extensions.

* `dart:html`, `dart:indexed_db`, `dart:svg`, `dart:web_audio`, `dart:web_gl`, `dart:web_sql`
  * The return type of some APIs changed from `double` to `num`. Dartium is now
    using
    JS interop for most operations. JS does not distinguish between numeric
    types, and will return a number as an int if it fits in an int. This will
    mostly cause an error if you assign to something typed `double` in
    checked mode. You may
    need to insert a `toDouble()` call or accept `num`. Examples of APIs that
    are affected include `Element.getBoundingClientRect` and
    `TextMetrics.width`.

* `dart:io`
  * **Breaking:** Secure networking has changed, replacing the NSS library
    with the BoringSSL library. `SecureSocket`, `SecureServerSocket`,
    `RawSecureSocket`,`RawSecureServerSocket`, `HttpClient`, and `HttpServer`
    now all use a `SecurityContext` object which contains the certificates
    and keys used for secure TLS (SSL) networking.

    This is a breaking change for server applications and for some client
    applications. Certificates and keys are loaded into the `SecurityContext`
    from PEM files, instead of from an NSS certificate database. Information
    about how to change applications that use secure networking is at
    https://www.dartlang.org/server/tls-ssl.html

  * `HttpClient` no longer sends URI fragments in the request. This is not
    allowed by the HTTP protocol.
    The `HttpServer` still gracefully receives fragments, but discards them
    before delivering the request.
  * To allow connections to be accepted on the same port across different
    isolates, set the `shared` argument to `true` when creating server socket
    and `HttpServer` instances.
    * The deprecated `ServerSocketReference` and `RawServerSocketReference`
      classes have been removed.
    * The corresponding `reference` properties on `ServerSocket` and
      `RawServerSocket` have been removed.

* `dart:isolate`
  * `spawnUri` added an `environment` named argument.

### Tool changes

* `dart2js` and Dartium now support improved Javascript Interoperability via the
  [js package](https://pub.dartlang.org/packages/js).

* `docgen` and `dartdocgen` no longer ship in the SDK. The `docgen` sources have
   been removed from the repository.

* This is the last release to ship the VM's "legacy debug protocol".
  We intend to remove the legacy debug protocol in Dart VM 1.14.

* The VM's Service Protocol has been updated to version 3.0 to take care
  of a number of issues uncovered by the first few non-observatory
  clients.  This is a potentially breaking change for clients.

* Dartium has been substantially changed. Rather than using C++ calls into
  Chromium internals for DOM operations it now uses JS interop.
  The DOM objects in `dart:html` and related libraries now wrap
  a JavaScript object and delegate operations to it. This should be
  mostly transparent to users. However, performance and memory characteristics
  may be different from previous versions. There may be some changes in which
  DOM objects are wrapped as Dart objects. For example, if you get a reference
  to a Window object, even through JS interop, you will always see it as a
  Dart Window, even when used cross-frame. We expect the change to using
  JS interop will make it much simpler to update to new Chrome versions.

## 1.12.2 - 2015-10-21

### Core library changes

* `dart:io`

  * A memory leak in creation of Process objects is fixed.

## 1.12.1 - 2015-09-08

### Tool changes

* Pub

  * Pub will now respect `.gitignore` when validating a package before it's
    published. For example, if a `LICENSE` file exists but is ignored, that is
    now an error.

  * If the package is in a subdirectory of a Git repository and the entire
    subdirectory is ignored with `.gitignore`, pub will act as though nothing
    was ignored instead of uploading an empty package.

  * The heuristics for determining when `pub get` needs to be run before various
    commands have been improved. There should no longer be false positives when
    non-dependency sections of the pubspec have been modified.

## 1.12.0 - 2015-08-31

### Language changes

* Null-aware operators
    * `??`: if null operator. `expr1 ?? expr2` evaluates to `expr1` if
      not `null`, otherwise `expr2`.
    * `??=`: null-aware assignment. `v ??= expr` causes `v` to be assigned
      `expr` only if `v` is `null`.
    * `x?.p`: null-aware access. `x?.p` evaluates to `x.p` if `x` is not
      `null`, otherwise evaluates to `null`.
    * `x?.m()`: null-aware method invocation. `x?.m()` invokes `m` only
      if `x` is not `null`.

### Core library changes

* `dart:async`
  * `StreamController` added setters for the `onListen`, `onPause`, `onResume`
    and `onCancel` callbacks.

* `dart:convert`
  * `LineSplitter` added a `split` static method returning an `Iterable`.

* `dart:core`
  * `Uri` class now perform path normalization when a URI is created.
    This removes most `..` and `.` sequences from the URI path.
    Purely relative paths (no scheme or authority) are allowed to retain
    some leading "dot" segments.
    Also added `hasAbsolutePath`, `hasEmptyPath`, and `hasScheme` properties.

* `dart:developer`
  * New `log` function to transmit logging events to Observatory.

* `dart:html`
  * `NodeTreeSanitizer` added the `const trusted` field. It can be used
    instead of defining a `NullTreeSanitizer` class when calling
    `setInnerHtml` or other methods that create DOM from text. It is
    also more efficient, skipping the creation of a `DocumentFragment`.

* `dart:io`
  * Added two new file modes, `WRITE_ONLY` and `WRITE_ONLY_APPEND` for
    opening a file write only.
    [eaeecf2](https://github.com/dart-lang/sdk/commit/eaeecf2ed13ba6c7fbfd653c3c592974a7120960)
  * Change stdout/stderr to binary mode on Windows.
    [4205b29](https://github.com/dart-lang/sdk/commit/4205b2997e01f2cea8e2f44c6f46ed6259ab7277)

* `dart:isolate`
  * Added `onError`, `onExit` and `errorsAreFatal` parameters to
    `Isolate.spawnUri`.

* `dart:mirrors`
  * `InstanceMirror.delegate` moved up to `ObjectMirror`.
  * Fix InstanceMirror.getField optimization when the selector is an operator.
  * Fix reflective NoSuchMethodErrors to match their non-reflective
    counterparts when due to argument mismatches. (VM only)

### Tool changes

* Documentation tools

  * `dartdoc` is now the default tool to generate static HTML for API docs.
    [Learn more](https://pub.dartlang.org/packages/dartdoc).

  * `docgen` and `dartdocgen` have been deprecated. Currently plan is to remove
    them in 1.13.

* Formatter (`dartfmt`)

  * Over 50 bugs fixed.

  * Optimized line splitter is much faster and produces better output on
    complex code.

* Observatory
  * Allocation profiling.

  * New feature to display output from logging.

  * Heap snapshot analysis works for 64-bit VMs.

  * Improved ability to inspect typed data, regex and compiled code.

  * Ability to break on all or uncaught exceptions from Observatory's debugger.

  * Ability to set closure-specific breakpoints.

  * 'anext' - step past await/yield.

  * Preserve when a variable has been expanded/unexpanded in the debugger.

  * Keep focus on debugger input box whenever possible.

  * Echo stdout/stderr in the Observatory debugger.  Standalone-only so far.

  * Minor fixes to service protocol documentation.

* Pub

  * **Breaking:** various commands that previously ran `pub get` implicitly no
    longer do so. Instead, they merely check to make sure the ".packages" file
    is newer than the pubspec and the lock file, and fail if it's not.

  * Added support for `--verbosity=error` and `--verbosity=warning`.

  * `pub serve` now collapses multiple GET requests into a single line of
    output. For full output, use `--verbose`.

  * `pub deps` has improved formatting for circular dependencies on the
    entrypoint package.

  * `pub run` and `pub global run`

    * **Breaking:** to match the behavior of the Dart VM, executables no longer
      run in checked mode by default. A `--checked` flag has been added to run
      them in checked mode manually.

    * Faster start time for executables that don't import transformed code.

    * Binstubs for globally-activated executables are now written in the system
      encoding, rather than always in `UTF-8`. To update existing executables,
      run `pub cache repair`.

  * `pub get` and `pub upgrade`

    * Pub will now generate a ".packages" file in addition to the "packages"
      directory when running `pub get` or similar operations, per the
      [package spec proposal][]. Pub now has a `--no-package-symlinks` flag that
      will stop "packages" directories from being generated at all.

    * An issue where HTTP requests were sometimes made even though `--offline`
      was passed has been fixed.

    * A bug with `--offline` that caused an unhelpful error message has been
      fixed.

    * Pub will no longer time out when a package takes a long time to download.

  * `pub publish`

    * Pub will emit a non-zero exit code when it finds a violation while
      publishing.

    * `.gitignore` files will be respected even if the package isn't at the top
      level of the Git repository.

  * Barback integration

    * A crashing bug involving transformers that only apply to non-public code
      has been fixed.

    * A deadlock caused by declaring transformer followed by a lazy transformer
      (such as the built-in `$dart2js` transformer) has been fixed.

    * A stack overflow caused by a transformer being run multiple times on the
      package that defines it has been fixed.

    * A transformer that tries to read a non-existent asset in another package
      will now be re-run if that asset is later created.

[package spec proposal]: https://github.com/lrhn/dep-pkgspec

### VM Service Protocol Changes

* **BREAKING** The service protocol now sends JSON-RPC 2.0-compatible
  server-to-client events. To reflect this, the service protocol version is
  now 2.0.

* The service protocol now includes a `"jsonrpc"` property in its responses, as
  opposed to `"json-rpc"`.

* The service protocol now properly handles requests with non-string ids.
  Numeric ids are no longer converted to strings, and null ids now don't produce
  a response.

* Some RPCs that didn't include a `"jsonrpc"` property in their responses now
  include one.

## 1.11.2 - 2015-08-03

### Core library changes

* Fix a bug where `WebSocket.close()` would crash if called after
  `WebSocket.cancel()`.

## 1.11.1 - 2015-07-02

### Tool changes

* Pub will always load Dart SDK assets from the SDK whose `pub` executable was
  run, even if a `DART_SDK` environment variable is set.

## 1.11.0 - 2015-06-25

### Core library changes

* `dart:core`
  * `Iterable` added an `empty` constructor.
    [dcf0286](https://github.com/dart-lang/sdk/commit/dcf0286f5385187a68ce9e66318d3bf19abf454b)
  * `Iterable` can now be extended directly. An alternative to extending
    `IterableBase` from `dart:collection`.
  * `List` added an `unmodifiable` constructor.
    [r45334](https://code.google.com/p/dart/source/detail?r=45334)
  * `Map` added an `unmodifiable` constructor.
    [r45733](https://code.google.com/p/dart/source/detail?r=45733)
  * `int` added a `gcd` method.
    [a192ef4](https://github.com/dart-lang/sdk/commit/a192ef4acb95fad1aad1887f59eed071eb5e8201)
  * `int` added a `modInverse` method.
    [f6f338c](https://github.com/dart-lang/sdk/commit/f6f338ce67eb8801b350417baacf6d3681b26002)
  * `StackTrace` added a `fromString` constructor.
    [68dd6f6](https://github.com/dart-lang/sdk/commit/68dd6f6338e63d0465041d662e778369c02c2ce6)
  * `Uri` added a `directory` constructor.
    [d8dbb4a](https://github.com/dart-lang/sdk/commit/d8dbb4a60f5e8a7f874c2a4fbf59eaf1a39f4776)
  * List iterators may not throw `ConcurrentModificationError` as eagerly in
    release mode. In checked mode, the modification check is still as eager
    as possible.
    [r45198](https://github.com/dart-lang/sdk/commit/5a79c03)

* `dart:developer` - **NEW**
  * Replaces the deprecated `dart:profiler` library.
  * Adds new functions `debugger` and `inspect`.
    [6e42aec](https://github.com/dart-lang/sdk/blob/6e42aec4f64cf356dde7bad9426e07e0ea5b58d5/sdk/lib/developer/developer.dart)

* `dart:io`
  * `FileSystemEntity` added a `uri` property.
    [8cf32dc](https://github.com/dart-lang/sdk/commit/8cf32dc1a1664b516e57f804524e46e55fae88b2)
  * `Platform` added a `static resolvedExecutable` property.
    [c05c8c6](https://github.com/dart-lang/sdk/commit/c05c8c66069db91cc2fd48691dfc406c818d411d)

* `dart:html`
  * `Element` methods, `appendHtml` and `insertAdjacentHtml` now take `nodeValidator`
    and `treeSanitizer` parameters, and the inputs are consistently
    sanitized.
    [r45818 announcement](https://groups.google.com/a/dartlang.org/forum/#!topic/announce/GVO7EAcPi6A)

* `dart:isolate`
  * **BREAKING** The positional `priority` parameter of `Isolate.ping` and `Isolate.kill` is
    now a named parameter named `priority`.
  * **BREAKING** Removed the `Isolate.AS_EVENT` priority.
  * `Isolate` methods `ping` and `addOnExitListener` now have a named parameter
    `response`.
    [r45092](https://github.com/dart-lang/sdk/commit/1b208bd)
  * `Isolate.spawnUri` added a named argument `checked`.
  * Remove the experimental state of the API.

* `dart:profiler` - **DEPRECATED**
  * This library will be removed in 1.12. Use `dart:developer` instead.

### Tool changes

* This is the first release that does not include the Eclipse-based
  **Dart Editor**.
  See [dartlang.org/tools](https://www.dartlang.org/tools/) for alternatives.
* This is the last release that ships the (unsupported)
  dart2dart (aka `dart2js --output-type=dart`) utility as part
  of dart2js

## 1.10.0 - 2015-04-29

### Core library changes

* `dart:convert`
  * **POTENTIALLY BREAKING** Fix behavior of `HtmlEscape`. It no longer escapes
  no-break space (U+00A0) anywhere or forward slash (`/`, `U+002F`) in element
  context. Slash is still escaped using `HtmlEscapeMode.UNKNOWN`.
  [r45003](https://github.com/dart-lang/sdk/commit/8b8223d),
  [r45153](https://github.com/dart-lang/sdk/commit/8a5d049),
  [r45189](https://github.com/dart-lang/sdk/commit/3c39ad2)

* `dart:core`
  * `Uri.parse` added `start` and `end` positional arguments.

* `dart:html`
  * **POTENTIALLY BREAKING** `CssClassSet` method arguments must now be 'tokens', i.e. non-empty
  strings with no white-space characters. The implementation was incorrect for
  class names containing spaces. The fix is to forbid spaces and provide a
  faster implementation.
  [Announcement](https://groups.google.com/a/dartlang.org/d/msg/announce/jmUI2XJHfC8/UZUCvJH3p2oJ)

* `dart:io`

  * `ProcessResult` now exposes a constructor.
  * `import` and `Isolate.spawnUri` now supports the
    [Data URI scheme](http://en.wikipedia.org/wiki/Data_URI_scheme) on the VM.

## Tool Changes

### pub

  * Running `pub run foo` within a package now runs the `foo` executable defined
    by the `foo` package. The previous behavior ran `bin/foo`. This makes it
    easy to run binaries in dependencies, for instance `pub run test`.

  * On Mac and Linux, signals sent to `pub run` and forwarded to the child
    command.

## 1.9.3 - 2015-04-14

This is a bug fix release which merges a number of commits from `bleeding_edge`.

* dart2js: Addresses as issue with minified Javascript output with CSP enabled -
  [r44453](https://code.google.com/p/dart/source/detail?r=44453)

* Editor: Fixes accidental updating of files in the pub cache during rename
  refactoring - [r44677](https://code.google.com/p/dart/source/detail?r=44677)

* Editor: Fix for
  [issue 23032](https://code.google.com/p/dart/issues/detail?id=23032)
  regarding skipped breakpoints on Windows -
  [r44824](https://code.google.com/p/dart/source/detail?r=44824)

* dart:mirrors: Fix `MethodMirror.source` when the method is on the first line
  in a script -
  [r44957](https://code.google.com/p/dart/source/detail?r=44957),
  [r44976](https://code.google.com/p/dart/source/detail?r=44976)

* pub: Fix for
  [issue 23084](https://code.google.com/p/dart/issues/detail?id=23084):
  Pub can fail to load transformers necessary for local development -
  [r44876](https://code.google.com/p/dart/source/detail?r=44876)

## 1.9.1 - 2015-03-25

### Language changes

* Support for `async`, `await`, `sync*`, `async*`, `yield`, `yield*`, and `await
  for`. See the [the language tour][async] for more details.

* Enum support is fully enabled. See [the language tour][enum] for more details.

[async]: https://www.dartlang.org/docs/dart-up-and-running/ch02.html#asynchrony
[enum]: https://www.dartlang.org/docs/dart-up-and-running/ch02.html#enums

### Tool changes

* The formatter is much more comprehensive and generates much more readable
  code. See [its tool page][dartfmt] for more details.

* The analysis server is integrated into the IntelliJ plugin and the Dart
  editor. This allows analysis to run out-of-process, so that interaction
  remains smooth even for large projects.

* Analysis supports more and better hints, including unused variables and unused
  private members.

[dartfmt]: https://www.dartlang.org/tools/dartfmt/

### Core library changes

#### Highlights

* There's a new model for shared server sockets with no need for a `Socket`
  reference.

* A new, much faster [regular expression engine][regexp].

* The Isolate API now works across the VM and `dart2js`.

[regexp]: http://news.dartlang.org/2015/02/irregexp-dart-vms-new-regexp.html

#### Details

For more information on any of these changes, see the corresponding
documentation on the [Dart API site](http://api.dartlang.org).

* `dart:async`:

  * `Future.wait` added a new named argument, `cleanUp`, which is a callback
    that releases resources allocated by a successful `Future`.

  * The `SynchronousStreamController` class was added as an explicit name for
    the type returned when the `sync` argument is passed to `new
    StreamController`.

* `dart:collection`: The `new SplayTreeSet.from(Iterable)` constructor was
  added.

* `dart:convert`: `Utf8Encoder.convert` and `Utf8Decoder.convert` added optional
  `start` and `end` arguments.

* `dart:core`:

  * `RangeError` added new static helper functions: `checkNotNegative`,
    `checkValidIndex`, `checkValidRange`, and `checkValueInInterval`.

  * `int` added the `modPow` function.

  * `String` added the `replaceFirstMapped` and `replaceRange` functions.

* `dart:io`:

  * Support for locking files to prevent concurrent modification was added. This
    includes the `File.lock`, `File.lockSync`, `File.unlock`, and
    `File.unlockSync` functions as well as the `FileLock` class.

  * Support for starting detached processes by passing the named `mode` argument
    (a `ProcessStartMode`) to `Process.start`. A process can be fully attached,
    fully detached, or detached except for its standard IO streams.

  * `HttpServer.bind` and `HttpServer.bindSecure` added the `v6Only` named
    argument. If this is true, only IPv6 connections will be accepted.

  * `HttpServer.bind`, `HttpServer.bindSecure`, `ServerSocket.bind`,
    `RawServerSocket.bind`, `SecureServerSocket.bind` and
    `RawSecureServerSocket.bind` added the `shared` named argument. If this is
    true, multiple servers or sockets in the same Dart process may bind to the
    same address, and incoming requests will automatically be distributed
    between them.

  * **Deprecation:** the experimental `ServerSocketReference` and
    `RawServerSocketReference` classes, as well as getters that returned them,
    are marked as deprecated. The `shared` named argument should be used
    instead. These will be removed in Dart 1.10.

  * `Socket.connect` and `RawSocket.connect` added the `sourceAddress` named
    argument, which specifies the local address to bind when making a
    connection.

  * The static `Process.killPid` method was added to kill a process with a given
    PID.

  * `Stdout` added the `nonBlocking` instance property, which returns a
    non-blocking `IOSink` that writes to standard output.

* `dart:isolate`:

  * The static getter `Isolate.current` was added.

  * The `Isolate` methods `addOnExitListener`, `removeOnExitListener`,
    `setErrorsFatal`, `addOnErrorListener`, and `removeOnErrorListener` now work
    on the VM.

  * Isolates spawned via `Isolate.spawn` now allow most objects, including
    top-level and static functions, to be sent between them.

## 1.8.5 - 2015-01-21

* Code generation for SIMD on ARM and ARM64 is fixed.

* A possible crash on MIPS with newer GCC toolchains has been prevented.

* A segfault when using `rethrow` was fixed ([issue 21795][]).

[issue 21795]: https://code.google.com/p/dart/issues/detail?id=21795

## 1.8.3 - 2014-12-10

* Breakpoints can be set in the Editor using file suffixes ([issue 21280][]).

* IPv6 addresses are properly handled by `HttpClient` in `dart:io`, fixing a
  crash in pub ([issue 21698][]).

* Issues with the experimental `async`/`await` syntax have been fixed.

* Issues with a set of number operations in the VM have been fixed.

* `ListBase` in `dart:collection` always returns an `Iterable` with the correct
  type argument.

[issue 21280]: https://code.google.com/p/dart/issues/detail?id=21280
[issue 21698]: https://code.google.com/p/dart/issues/detail?id=21698

## 1.8.0 - 2014-11-28

* `dart:collection`: `SplayTree` added the `toSet` function.

* `dart:convert`: The `JsonUtf8Encoder` class was added.

* `dart:core`:

  * The `IndexError` class was added for errors caused by an index being outside
    its expected range.

  * The `new RangeError.index` constructor was added. It forwards to `new
    IndexError`.

  * `RangeError` added three new properties. `invalidProperty` is the value that
    caused the error, and `start` and `end` are the minimum and maximum values
    that the value is allowed to assume.

  * `new RangeError.value` and `new RangeError.range` added an optional
    `message` argument.

  * The `new String.fromCharCodes` constructor added optional `start` and `end`
    arguments.

* `dart:io`:

  * Support was added for the [Application-Layer Protocol Negotiation][alpn]
    extension to the TLS protocol for both the client and server.

  * `SecureSocket.connect`, `SecureServerSocket.bind`,
    `RawSecureSocket.connect`, `RawSecureSocket.secure`,
    `RawSecureSocket.secureServer`, and `RawSecureServerSocket.bind` added a
    `supportedProtocols` named argument for protocol negotiation.

  * `RawSecureServerSocket` added a `supportedProtocols` field.

  * `RawSecureSocket` and `SecureSocket` added a `selectedProtocol` field which
    contains the protocol selected during protocol negotiation.

[alpn]: https://tools.ietf.org/html/rfc7301

## 1.7.0 - 2014-10-15

### Tool changes

* `pub` now generates binstubs for packages that are globally activated so that
  they can be put on the user's `PATH` and used as normal executables. See the
  [`pub global activate` documentation][pub global activate].

* When using `dart2js`, deferred loading now works with multiple Dart apps on
  the same page.

[pub global activate]: https://www.dartlang.org/tools/pub/cmd/pub-global.html#running-a-script-from-your-path

### Core library changes

* `dart:async`: `Zone`, `ZoneDelegate`, and `ZoneSpecification` added the
  `errorCallback` function, which allows errors that have been programmatically
  added to a `Future` or `Stream` to be intercepted.

* `dart:io`:

  * **Breaking change:** `HttpClient.close` must be called for all clients or
    they will keep the Dart process alive until they time out. This fixes the
    handling of persistent connections. Previously, the client would shut down
    immediately after a request.

  * **Breaking change:** `HttpServer` no longer compresses all traffic by
    default. The new `autoCompress` property can be set to `true` to re-enable
    compression.

* `dart:isolate`: `Isolate.spawnUri` added the optional `packageRoot` argument,
  which controls how it resolves `package:` URIs.
