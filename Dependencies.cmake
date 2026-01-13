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
    DEPENDENCY_NAME         unity
    DEPENDENCY_RESOURCE     ThrowTheSwitch/Unity
    DEPENDENCY_VERSION      v2.6.1
    DEPENDENCY_SCOPE        TEST
)
