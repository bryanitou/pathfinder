#!/bin/bash
# Pathfinder deployment script
# Generates a stand-alone version of Pathfinder, including all the required libraries
# Usage: bash ./scripts/deploy.sh --log-max-level <verbosity_limit> --targets <executable_name> <executable_name> ...
# Example: bash ./scripts/deploy.sh --log-max-level FATAL --targets duavc_planning geometric_car"
# Exit codes: 0 -> ok, deployment completed / 1 -> error, deployment failed

# Function to display ASCII art logo
# You can find it here: https://patorjk.com/software/taag/#p=display&f=Letters&t=PATHFINDER
function display_logo()
{
  echo "PPPPPP    AAA   TTTTTTT HH   HH FFFFFFF IIIII NN   NN DDDDD   EEEEEEE RRRRRR  "
  echo "PP   PP  AAAAA    TTT   HH   HH FF       III  NNN  NN DD  DD  EE      RR   RR "
  echo "PPPPPP  AA   AA   TTT   HHHHHHH FFFF     III  NN N NN DD   DD EEEEE   RRRRRR  "
  echo "PP      AAAAAAA   TTT   HH   HH FF       III  NN  NNN DD   DD EE      RR  RR  "
  echo "PP      AA   AA   TTT   HH   HH FF      IIIII NN   NN DDDDDD  EEEEEEE RR   RR "
}

# Show script tutorial if required
function print_usage()
{
  echo "Usage: bash ./scripts/$(basename "$0")" \
       "[--log-max-level FATAL*|ERROR|WARNING|INFO|LOG|DEBUG|TRACE] [-j <number_of_threads>]" \
       "[--all | --target <executable_name> | --targets <executable_name> <executable_name> ...]"

  echo " "
  echo "Script must be called from dynamo3/ folder! Otherwise source directory will not be found!"
  echo " "

  echo "Default log cap is FATAL."
  echo "Default targets is all (deploys all Dynamo executables)."
  echo "Imposed build type is Release and build directory is <cwd>/build/dynamo3-build-release."
  echo " "

  echo "--> Example: bash ./scripts/deploy.sh --log-max-level FATAL --targets dynamo3_hachi_c3po dynamo3_hachi_r2d2"
  echo "--> The script will output all the release files into: ./release/<git_commit_date>"
  echo "--> A sample launch script template (launch.sh) will be provided in the release directory."
  exit 1 # Stop execution and return to terminal
}

# Function to get all the available cores: $(n_cores)
function n_cores()
{
  # Note: getconf is more portable than nproc
  #echo $(nproc --all)
  #echo "$(getconf _NPROCESSORS_ONLN)"
  getconf _NPROCESSORS_ONLN
}

# Print Dynamo3 header
function version_info()
{
  release="$1"
  # Banner
  echo "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
  display_logo
  echo " "

  # Sudo easter egg
  if [ "$EUID" -eq 0 ]; then
    echo "Gokigen'yō, Goshujin-sama~"
    echo " "
  fi

  # Info
  version=$(cat "VERSION")
  branch=$(git rev-parse --abbrev-ref HEAD)
  hash=$(git rev-parse HEAD)
  user=$(whoami)
  date=$(date +"%b %d %Y, %T")
  machine=$(uname -srpo)
  echo "PATHFINDER RELEASE ${release}"
  echo "Dyn version: ${version}"
  echo "Repo branch: ${branch}"
  echo "Commit hash: ${hash}"
  echo "Compiled by: ${user}"
  echo "Compiled on: ${date}"
  echo "Compiled on: ${machine}"
  echo "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
  echo ""
}

# Echo all available binaries inside a directory
function available_binaries()
{
  dir="$1"

  # Find the binaries inside the exec_dir (avoid listing this script)
  available_binaries=$(find "${dir}/" -mindepth 1 -maxdepth 1 -type f \
                            ! -name 'launch.sh' ! -name 'LICENSE' ! -name 'README.md' ! -name 'RELEASE')

  # Echo actually available binaries
  for available_binary in ${available_binaries[*]}; do
    basename "${available_binary}"
  done
}

# Prints RELEASE inside the release directory
function create_info_file()
{
  rel_dir="$1"
  rel_ID=$(basename "$1")
  file="${rel_dir}/RELEASE"

  # Create file
  touch "${file}"

  # Print header
  version_info "${rel_ID}" | tee -a "${file}"

  # Print deployed binaries
  echo "THE FOLLOWING BINARIES HAVE BEEN DEPLOYED: " | tee -a "${file}"
  available_binaries "${rel_dir}" | tee -a "${file}"
}

# Read a single section of a markdown file
function read_markdown_section()
{
  file="$1"
  title="$2"

  sed "/^${title}$/,/^## /!d;//d;" "${file}"
}

# Write a single section into a markdown file
function write_markdown_section()
{
  file="$1"
  title="$2"
  content="$3"

  echo "${title}" | tee -a "${file}" >/dev/null
  echo "${content}" | tee -a "${file}" >/dev/null
  echo ""  | tee -a "${file}" >/dev/null
}

# Print a new reduced README.md for end users
function release_README()
{
  original_README="$1"
  new_README="$2"

  # Create new README
  touch "${new_README}"

  # Read important sections of original README
  description=$(read_markdown_section "${original_README}" "# PATHFINDER")
  usage=$(read_markdown_section "${original_README}" "## End user usage")
  authors=$(read_markdown_section "${original_README}" "## Authors")
  citation=$(read_markdown_section "${original_README}" "## Citation")
  license=$(read_markdown_section "${original_README}" "## License")

  # Write new readme
  write_markdown_section "${new_README}" "# PATHFINDER" "${description}"
  write_markdown_section "${new_README}" "## End user usage" "${usage}"
  write_markdown_section "${new_README}" "## Authors" "${authors}"
  write_markdown_section "${new_README}" "## Citation" "${citation}"
  write_markdown_section "${new_README}" "## License" "${license}"
}

# Default parameters
current_dir=$(realpath .)
verbosity_limit="FATAL"

source_dir="${current_dir}"
build_dir="${current_dir}/build/pathfinder-build-release"
scripts_dir="${source_dir}/scripts"
file_launch="launch.sh"

# Release date (this is the date of the current git commit)
release_date=$(git log -1 --date="format:%Y%m%d%H%M%S" --format="%ad")

# Build release directory name
out_dir="${current_dir}/release"
release_dir="${out_dir}/pathfinder_v$(cat "VERSION")_${release_date}"

# What to build
build_all=true
build_targets=()
threads=$(n_cores)

# Get parameters
while test $# != 0; do
  case "$1" in
  -v | --verbosity | --log-max-level)
    verbosity_limit="$2"
    shift
    ;;
  -j | --jobs)
    threads="$2"
    shift
    ;;
  -a | --all) build_all=true ;;
  -t | --target | --targets)
    build_all=false
    build_targets=()

    # Iterate until next flag
    for val in "${@:2}"; do
      if [[ "${val}" == -* ]]; then
        break
      else
        build_targets+=("${val}")
      fi
    done
    shift "${#build_targets[@]}"
    ;;
  --)
    shift
    break
    ;;
  *) print_usage ;;
  esac
  shift
done

# Welcome message
if [ "${SHLVL}" -lt 3 ]; then
  display_logo
  echo " "
  echo "WELCOME TO PATHFINDER COMPILATION SCRIPT"
  echo "This script will build all PATHFINDER programs and dependencies from scratch"
  echo "and it will package them in a release folder."
  echo " "
fi

# Build binaries with optional logger limit
echo -n "RECOMPILING PATHFINDER WITH VERBOSITY LIMIT: ${verbosity_limit}. PLEASE WAIT... "
if [ "${build_all}" = true ]; then
  if messages=$(bash "./scripts/build_all.sh" --clean-dirs --release --verbosity "${verbosity_limit}" -j "${threads}" \
           --source-dir "${source_dir}" --build-dir "${build_dir}" --install-dir "${release_dir}" \
           --all 2>&1); then
    echo "OK."
  else
    exit_code="$?"
    echo " "
    echo "${messages}" | tail -10
    echo "ERROR during compilation. Error code: ${exit_code}"
    exit "${exit_code}"
  fi
else
  if messages=$(bash "./scripts/build_all.sh" --clean-dirs --release --verbosity "${verbosity_limit}" -j "${threads}" \
           --source-dir "${source_dir}" --build-dir "${build_dir}" --install-dir "${release_dir}" \
           --targets "${build_targets[@]}" 2>&1); then
    echo "OK."
  else
    exit_code="$?"
    echo " "
    echo "${messages}" | tail -10
    echo "ERROR during compilation. Error code: ${exit_code}"
    exit "${exit_code}"
  fi
fi

echo " "

# Search for NetCDF dependencies and copy them to the release
echo "COPYING SYSTEM LIBRARIES..."
file_lib_netcdf="libnetcdf.so.19"
file_lib_hdf5="libhdf5_serial.so"
file_lib_hdf5_hl="libhdf5_serial_hl.so"
file_lib_z="libsz.so"
file_lib_aec="libaec.so"
dir_lib_hdf5_hl="$(ldd "${release_dir}/lib/${file_lib_netcdf}" | grep "${file_lib_hdf5_hl}" | tr ' ' '\n' | grep /)"
dir_lib_hdf5="$(ldd "${release_dir}/lib/${file_lib_netcdf}" | grep "${file_lib_hdf5}" | tr ' ' '\n' | grep /)"
dir_lib_z="$(ldd "${release_dir}/lib/${file_lib_netcdf}" | grep "${file_lib_z}" | tr ' ' '\n' | grep /)"
dir_lib_aec="$(ldd "${release_dir}/lib/${file_lib_netcdf}" | grep "${file_lib_aec}" | tr ' ' '\n' | grep /)"
cp -L -rfv "${dir_lib_hdf5}" "${release_dir}/lib/"
cp -L -rfv "${dir_lib_hdf5_hl}" "${release_dir}/lib/"
cp -L -rfv "${dir_lib_z}" "${release_dir}/lib/"
cp -L -rfv "${dir_lib_aec}" "${release_dir}/lib/"
echo " "

# Copy sample launch script
echo "COPYING SAMPLE LAUNCH SCRIPT..."
cp -rfv "${scripts_dir}/${file_launch}" "${release_dir}/${file_launch}"
echo " "

# Release info
echo "PRINTING RELEASE INFO: "
create_info_file "${release_dir}"
echo " "

# New readme
echo "PRINTING RELEASE README..."

# Ignore development sections, copy end user info
release_README "${source_dir}/README.md" "${release_dir}/README.md"
echo "Created ${release_dir}/README.md"

# Copy license file
cp -L -rfv "${source_dir}/LICENSE" "${release_dir}/"

echo " "

# Info
echo "ALL DONE!"
echo " "
echo "PATHFINDER RELEASE READY:"
echo "--> ${release_dir}/"
echo "--> Use the sample launch script to customize your application."
echo " "

# Exit
exit 0
