# Examples

This page shows example code of how to use the C API provided by `libreckon`.

## C API Example

The following source code is a complete example of how one can use the Reckon library.

```c
#include <stdbool.h>
#include <stdio.h>

#include "reckon/reckon.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PATH>\n", argv[0]);
        return 1;
    }

    RcnCountStatistics* stats = rcnCreateCountStatistics(argv[1]);
    if (!stats) {
        fprintf(stderr, "Failed to create statistics\n");
        return 2;
    }

    RcnStatOptions options = {
        .stopOnError = true
    };

    rcnCount(stats, options);

    if (!stats->state.ok) {
        fprintf(stderr, "Counting failed (error=%d)\n", stats->state.errorCode);
        rcnFreeCountStatistics(stats);
        return 3;
    }

    printf("Total LLC: %lu\n", stats->totalLogicalLines);
    printf("Total PHL: %lu\n", stats->totalPhysicalLines);
    printf("Total WRD: %lu\n", stats->totalWords);
    printf("Total CHR: %lu\n", stats->totalCharacters);
    printf("Total SZE: %lu bytes\n", stats->totalSourceSize);

    rcnFreeCountStatistics(stats);
    return 0;
}
```

If you need more control (e.g. count only C sources, or only LLC), set `options.formats` and/or `options.operations` using the `RCN_OPT_*` bitmasks described in the API reference.
