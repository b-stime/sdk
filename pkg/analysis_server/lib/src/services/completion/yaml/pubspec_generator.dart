// Copyright (c) 2020, the Dart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

import 'package:analysis_server/src/protocol_server.dart';
import 'package:analysis_server/src/services/completion/yaml/producer.dart';
import 'package:analysis_server/src/services/completion/yaml/yaml_completion_generator.dart';
import 'package:analysis_server/src/services/pub/pub_package_service.dart';
import 'package:analyzer/file_system/file_system.dart';

/// An object that represents the location of a package name.
class PubPackageNameProducer extends Producer {
  const PubPackageNameProducer();

  @override
  Iterable<CompletionSuggestion> suggestions(
      YamlCompletionRequest request) sync* {
    final cachedPackages = request.pubPackageService?.cachedPackages;
    if (cachedPackages != null) {
      var relevance = cachedPackages.length;
      yield* cachedPackages.map((package) =>
          packageName('${package.packageName}: ', relevance: relevance--));
    }
  }
}

/// A completion generator that can produce completion suggestions for pubspec
/// files.
class PubspecGenerator extends YamlCompletionGenerator {
  /// The producer representing the known valid structure of a pubspec file.
  static const MapProducer pubspecProducer = MapProducer({
    'name': EmptyProducer(),
    'version': EmptyProducer(),
    'description': EmptyProducer(),
    'homepage': EmptyProducer(),
    'repository': EmptyProducer(),
    'issue_tracker': EmptyProducer(),
    'documentation': EmptyProducer(),
    'executables': EmptyProducer(),
    'publish_to': EmptyProducer(),
    'environment': MapProducer({
      'flutter': EmptyProducer(),
      'sdk': EmptyProducer(),
    }),
    'dependencies': PubPackageNameProducer(),
    'dev_dependencies': PubPackageNameProducer(),
    // TODO(brianwilkerson) Suggest names already listed under 'dependencies'
    //  and 'dev_dependencies'.
    'dependency_overrides': EmptyProducer(),
    'flutter': MapProducer({
      'assets': ListProducer(FilePathProducer()),
      'fonts': ListProducer(MapProducer({
        'family': EmptyProducer(),
        'fonts': ListProducer(MapProducer({
          'asset': EmptyProducer(),
          'style': EnumProducer(['italic', 'normal']),
          'weight': EnumProducer(
              ['100', '200', '300', '400', '500', '600', '700', '800', '900']),
        })),
      })),
      'generate': BooleanProducer(),
      'module': MapProducer({
        'androidX': BooleanProducer(),
        'androidPackage': EmptyProducer(),
        'iosBundleIdentifier': EmptyProducer(),
      }),
      'plugin': MapProducer({
        'platforms': MapProducer({
          'android': MapProducer({
            'package': EmptyProducer(),
            'pluginClass': EmptyProducer(),
          }),
          'ios': MapProducer({
            'pluginClass': EmptyProducer(),
          }),
          'linux': MapProducer({
            'dartPluginClass': EmptyProducer(),
            'pluginClass': EmptyProducer(),
          }),
          'macos': MapProducer({
            'dartPluginClass': EmptyProducer(),
            'pluginClass': EmptyProducer(),
          }),
          'web': MapProducer({
            'fileName': EmptyProducer(),
            'pluginClass': EmptyProducer(),
          }),
          'windows': MapProducer({
            'dartPluginClass': EmptyProducer(),
            'pluginClass': EmptyProducer(),
          }),
        }),
      }),
      'uses-material-design': BooleanProducer(),
    }),
  });

  /// Initialize a newly created suggestion generator for pubspec files.
  PubspecGenerator(
      ResourceProvider resourceProvider, PubPackageService pubPackageService)
      : super(resourceProvider, pubPackageService);

  @override
  Producer get topLevelProducer => pubspecProducer;
}
