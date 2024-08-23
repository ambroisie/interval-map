{
  description = "Interval Map coding exercise";

  inputs = {
    futils = {
      type = "github";
      owner = "numtide";
      repo = "flake-utils";
      ref = "main";
    };

    nixpkgs = {
      type = "github";
      owner = "NixOS";
      repo = "nixpkgs";
      ref = "nixos-unstable";
    };

    pre-commit-hooks = {
      type = "github";
      owner = "cachix";
      repo = "pre-commit-hooks.nix";
      ref = "master";
      inputs = {
        flake-utils.follows = "futils";
        nixpkgs.follows = "nixpkgs";
      };
    };
  };

  outputs = { self, futils, nixpkgs, pre-commit-hooks }:
    {
      overlays = {
        default = final: _prev: {
          interval-map = with final; stdenv.mkDerivation {
            pname = "interval-map";
            version = "0.0.0";

            src = self;

            nativeBuildInputs = with pkgs; [
              meson
              ninja
              pkg-config
            ];

            checkInputs = with pkgs; [
              gtest
            ];

            doCheck = true;

            meta = with lib; {
              description = "An implementation of interval_map";
              homepage = "https://git.belanyi.fr/ambroisie/interval-map";
              license = licenses.mit;
              maintainers = with maintainers; [ ambroisie ];
              platforms = platforms.unix;
            };
          };
        };
      };
    } // futils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [
            self.overlays.default
          ];
        };

        pre-commit = pre-commit-hooks.lib.${system}.run {
          src = self;

          hooks = {
            nixpkgs-fmt = {
              enable = true;
            };

            clang-format = {
              enable = true;
            };
          };
        };
      in
      {
        checks = {
          inherit (self.packages.${system}) interval-map;

          inherit pre-commit;
        };

        devShells = {
          default = pkgs.mkShell {
            inputsFrom = with self.packages.${system}; [
              interval-map
            ];

            packages = with pkgs; [
              clang-tools
            ];

            inherit (pre-commit) shellHook;
          };
        };

        packages = futils.lib.flattenTree {
          default = pkgs.interval-map;
          inherit (pkgs) interval-map;
        };
      });
}
