{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  buildInputs = with pkgs; [
    # Core
    gnumake
    ripgrep
    rnix-lsp

    # Editing
    neovim
    ccls

    # C++
    bear
    clang
    sfml
  ];
}
