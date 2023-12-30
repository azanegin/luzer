# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Integration with libFuzzer's `LLVMFuzzerTestOneInput()`.
- Integration with libFuzzer's `LLVMFuzzerCustomMutator()`.
- Integration with libFuzzer's `FuzzedDataProvider`.
- Examples with tests.
- Documentation with usecases, API etc.
- Support for command line arguments for libfuzzer.
- Environment variable to disable parsing of command line arguments for libfuzzer - `LUZER_NOT_USE_CLI_ARGS_FOR_LF`.
- Two ways to approximate amount of counters for interpreted code.

### Fixed
- Interfering coverage instrumentation of fuzzer internals (#11)
- Interpreted code counter never handed to libfuzzer. (#12)
- Bad lifetime and initization of struct sigaction.
