include(cmake/DependencyUtil.cmake)

dependency(
    DEPENDENCY_NAME         tree-sitter
    DEPENDENCY_RESOURCE     tree-sitter/tree-sitter
    DEPENDENCY_VERSION      v0.26.3
    DEPENDENCY_LINK_TARGETS tree-sitter
)

# Use our own forked versions of the tree-sitter parser implementations
# to include a patched CMakeLists.txt version.
# This should be changed to use upstream versions once the patches are merged.

dependency(
    DEPENDENCY_NAME         tree-sitter-c
    DEPENDENCY_RESOURCE     kilo52/tree-sitter-c
    DEPENDENCY_VERSION      patch-cmakeliststxt
    DEPENDENCY_LINK_TARGETS tree-sitter-c
)

dependency(
    DEPENDENCY_NAME         tree-sitter-java
    DEPENDENCY_RESOURCE     kilo52/tree-sitter-java
    DEPENDENCY_VERSION      patch-cmakeliststxt
    DEPENDENCY_LINK_TARGETS tree-sitter-java
)

dependency(
    DEPENDENCY_NAME         tree-sitter-python
    DEPENDENCY_RESOURCE     kilo52/tree-sitter-python
    DEPENDENCY_VERSION      v0.25.0-patch-cmakeliststxt
    DEPENDENCY_LINK_TARGETS tree-sitter-python
)

dependency(
    DEPENDENCY_NAME         tree-sitter-javascript
    DEPENDENCY_RESOURCE     kilo52/tree-sitter-javascript
    DEPENDENCY_VERSION      v0.25.0-patch-cmakeliststxt
    DEPENDENCY_LINK_TARGETS tree-sitter-javascript
)

dependency(
    DEPENDENCY_NAME         tree-sitter-bash
    DEPENDENCY_RESOURCE     kilo52/tree-sitter-bash
    DEPENDENCY_VERSION      v0.25.1-patch-cmakeliststxt
    DEPENDENCY_LINK_TARGETS tree-sitter-bash
)

dependency(
    DEPENDENCY_NAME         tree-sitter-typescript
    DEPENDENCY_RESOURCE     kilo52/tree-sitter-typescript
    DEPENDENCY_VERSION      v0.23.2-patch-cmakeliststxt
    DEPENDENCY_LINK_TARGETS tree-sitter-typescript
)

dependency(
    DEPENDENCY_NAME         unity
    DEPENDENCY_RESOURCE     ThrowTheSwitch/Unity
    DEPENDENCY_VERSION      v2.6.1
    DEPENDENCY_SCOPE        TEST
)
