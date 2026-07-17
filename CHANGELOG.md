# Changelog

## [0.2.0](https://github.com/jwidess/esp-async-console/compare/v0.1.0...v0.2.0) (2026-07-17)


### Features

* 3 state terminal mode instead of dumbmode toggle ([0c497c1](https://github.com/jwidess/esp-async-console/commit/0c497c16ecc48b56f537e86e3eba317827967982))
* Auto smart/dumb terminal detection and fix parsing crash ([e6e3180](https://github.com/jwidess/esp-async-console/commit/e6e31807972a31a046e63002e1b24e9d85569e3d))
* auto upgrade to smart mode on any ESC byte ([0f6d532](https://github.com/jwidess/esp-async-console/commit/0f6d5329626c87c8c5f543bf81ffa8e8079a07a3))
* **example:** Custom tab completion for command arguments ([4494294](https://github.com/jwidess/esp-async-console/commit/4494294010d51706acafa5fa2eac43d9ba4452e6))
* Kconfig for console max length and argument count ([64c62e3](https://github.com/jwidess/esp-async-console/commit/64c62e3f2aac43b82fff339c58631ee22dd948cc))
* support Home/End keys in VT220 ([d7aa280](https://github.com/jwidess/esp-async-console/commit/d7aa280e2f811c4b7f187a2b8ea888b7a2dd41a6))
* Terminal mode switch messages ([c401cd2](https://github.com/jwidess/esp-async-console/commit/c401cd20f1c60c9dafd20280647087f95a0e5ae4))


### Bug Fixes

* async log redraw active tab completion ([e3f81aa](https://github.com/jwidess/esp-async-console/commit/e3f81aa39a27af8a3814e0342a620af5212c27ef))
* bypass getColumns() to prevent ANSI terminal probes causing garbage input ([9946f9b](https://github.com/jwidess/esp-async-console/commit/9946f9b170ced8dfc7d7dafde5a3f2bd633c3ad7))
* correct regex corruption and macro placement in linenoise header ([cfca0ae](https://github.com/jwidess/esp-async-console/commit/cfca0aed92e12ab5e02b7bdaa2591eb00dba389b))
* move examples idf_component.yml to main component ([5b27901](https://github.com/jwidess/esp-async-console/commit/5b279017f6b0b955c17c263af60e8076c67d7208))
* Off-by-one history bug and max args behavior ([fa429c5](https://github.com/jwidess/esp-async-console/commit/fa429c57f177754f868c0bfbd560f4b67d6c6a59))
* revert to reliable single line prompt ([db57e9f](https://github.com/jwidess/esp-async-console/commit/db57e9f12b0565407745f62356167eb3266196ef))
* Stray prompt in dumbmode and removed unused var ([a5f2fd2](https://github.com/jwidess/esp-async-console/commit/a5f2fd2cc456824070ca27f782aa2c29806be643))
