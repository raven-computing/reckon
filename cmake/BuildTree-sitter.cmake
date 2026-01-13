# Build instructions for the Tree-Sitter dependency

target_compile_definitions(
    tree-sitter
    PRIVATE
    TREE_SITTER_HIDE_SYMBOLS
    TREE_SITTER_HIDDEN_SYMBOLS
)
