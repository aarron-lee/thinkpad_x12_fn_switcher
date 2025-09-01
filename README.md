# Thinkpad x12 detachable gen 1 FN Switcher for Linux

- [Quick Install](#quick-install)
- [Disclaimer](#disclaimer)
- [Attribution](#attribution)

## Quick install

Note that the quick install uses python and all dependencies are provided, so this should work on immutable distros like Fedora Silverblue, Universal Blue variants, etc.

```
curl -L https://github.com/aarron-lee/thinkpad_x12_fn_switcher/raw/main/install.sh | sh
```

To uninstall, run:

```
curl -L https://github.com/aarron-lee/thinkpad_x12_fn_switcher/raw/main/uninstall.sh | sh
```

## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND...

## Attribution

Big thanks to upstream [manueljaeckle](https://github.com/manueljaeckle/thinkpad_x12_fn_switcher) for the initial implementation

Vendored dependency: https://github.com/apmorton/pyhidapi

Dependency vendored due to both python [hidapi](https://trezor.github.io/cython-hidapi/index.html) and [hid](https://github.com/apmorton/pyhidapi) using the same import signature, which is "import hid". Rather than deal with dependency conflicts, easier to just vendor the dependency
