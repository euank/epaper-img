{
  description = "devshell";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
        lgpio = pkgs.stdenv.mkDerivation {
          pname = "lgpio";
          version = "746f0df43774175090b93abcc860b6733eefc09b";
          src = pkgs.fetchFromGitHub {
            owner = "joan2937";
            repo = "lg";
            rev = "746f0df43774175090b93abcc860b6733eefc09b";
            hash = "sha256-ZGlIbfLOAawrY4E1Tzkf4L+MCZAG3Vu8wH4UW02x960=";
          };

          buildPhase = ''
            make lib
          '';
          installPhase = ''
            mkdir -p $out/lib
            mkdir -p $out/include
            cp -R *.h $out/include
            cp -R *.so* $out/lib

            mkdir -p $out/lib/pkgconfig
            cat > $out/lib/pkgconfig/lgpio.pc <<EOF
              Name: lgpio
              Description: lgpio
              Version: 1.0.0
              Cflags: -I$out/include
              Libs: -L$out/lib -llgpio
            EOF
          '';
        };
      in
      {
        formatter = pkgs.nixfmt-rfc-style;
        devShells.default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            clang-tools
            meson
            ninja
          ];
          buildInputs = [ lgpio ];

          PKG_CONFIG_PATH = "${lgpio}/lib/pkgconfig";
        };
      }
    );
}
