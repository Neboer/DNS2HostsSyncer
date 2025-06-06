{
  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
  outputs = { self, nixpkgs }:
    let
      supportedSystems = [
        "aarch64-linux"
        "aarch64-darwin"
        "i686-linux"
        "riscv64-linux"
        "x86_64-linux"
        "x86_64-darwin"
      ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
      nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; });
      commonDeps = (p: with p; [ nlohmann_json spdlog argparse curl ]);
    in {
      packages = forAllSystems (system:
        let pkgs = nixpkgsFor.${system};
        in {
          default = pkgs.stdenv.mkDerivation {
            pname = "d2hs";
            version = "4.1";
            src = ./.;
            nativeBuildInputs = (with pkgs; [ cmake pkg-config ]);
            buildInputs = (commonDeps pkgs) ++ [ ];
            enableParallelBuilding = true;
          };
        });

      devShells = forAllSystems (system:
        let pkgs = nixpkgsFor.${system};
        in {
          default = pkgs.mkShell {
            buildInputs =
              (with pkgs; [ meson ninja pkg-config stdenv.cc.cc clang-tools strace tree ])
              ++ (commonDeps pkgs);
          };
        });

      formatter = forAllSystems (system: nixpkgsFor.${system}.nixfmt);
    };
}

