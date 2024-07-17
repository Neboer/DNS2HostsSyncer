{
  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
  inputs.restclient-cpp-src = {
    url = "github:mrtazz/restclient-cpp";
    flake = false;
  };
  outputs = { self, nixpkgs, restclient-cpp-src }:
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
        let
          pkgs = nixpkgsFor.${system};
        in
        rec {
	  restclient-cpp = pkgs.stdenv.mkDerivation {
            pname = "restclient-cpp";
	        version = "git";
            src = restclient-cpp-src;
            nativeBuildInputs = with pkgs; [  gnumake cmake pkg-config ];
            buildInputs = with pkgs; [ curl ];
            configurePhase = ''
              mkdir $out
              cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$out
            '';
            buildPhase = ''
              cmake --build build
            '';
            installPhase = ''
              cmake --install build
            '';
          };

          default = pkgs.stdenv.mkDerivation {
            pname = "d2hs";
	        version = "1.0";
            src = ./.;
            nativeBuildInputs = (with pkgs; [ meson ninja cmake pkg-config ]);
            buildInputs = (commonDeps pkgs) ++ [ restclient-cpp ];
            enableParallelBuilding = true;
          };
        });

      devShells = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
        in
        {
          default = pkgs.mkShell {
            buildInputs = ( with pkgs; [
              meson
              ninja
              pkg-config
              clang
	      clang-tools
	      strace
            ]) ++ (commonDeps pkgs);
          };
        });
    };
}

