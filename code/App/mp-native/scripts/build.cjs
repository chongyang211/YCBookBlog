#!/usr/bin/env node
/*
 * scripts/build.cjs —— mp_native 统一构建入口（跨平台，替代 *.sh / *.ps1）
 *
 * 用法：
 *   node scripts/build.cjs package  [--taglib] [--sysmedia] [--universal]
 *       当前平台 install 包 → out/dist/（include + lib + CMake 包）
 *
 *   node scripts/build.cjs multi    [--no-docker] [--no-win] [--taglib] [--sysmedia]
 *       多平台产物 → out/multi/<platform-arch>/（darwin 本机双架构；linux/win 经 Docker）
 *
 *   node scripts/build.cjs smoke
 *       构建并运行纯 C 冒烟示例（mp_smoke）
 *
 * 说明：Docker 容器内构建由 scripts/container-build.sh 承担（容器无 Node），
 *       本脚本负责编排（docker run/cp/exec）。
 */
const { execFileSync, spawnSync } = require('node:child_process');
const fs = require('node:fs');
const path = require('node:path');
const os = require('node:os');

const ROOT = path.resolve(__dirname, '..');
const OUT = path.join(ROOT, 'out');

const argv = process.argv.slice(2);
const CMD = argv[0];
const has = (f) => argv.slice(1).includes(f);

const LIBNAME =
  process.platform === 'win32' ? 'mp_native.dll'
  : process.platform === 'darwin' ? 'libmp_native.dylib'
  : 'libmp_native.so';

/* ---------- 通用工具 ---------- */

function run(cmd, args = [], opts = {}) {
  console.log(`\n$ ${cmd} ${args.join(' ')}`);
  const r = spawnSync(cmd, args, { stdio: 'inherit', cwd: ROOT, ...opts });
  if (r.status !== 0) {
    console.error(`\n✗ ${cmd} 失败（exit ${r.status}）`);
    process.exit(r.status ?? 1);
  }
}

function runCapture(cmd, args, opts = {}) {
  const r = spawnSync(cmd, args, { cwd: ROOT, encoding: 'utf8', ...opts });
  return r.status === 0 ? r.stdout : null;
}

function hasCommand(cmd) {
  const probe = process.platform === 'win32' ? 'where' : 'which';
  return spawnSync(probe, [cmd]).status === 0;
}

function libName() { return LIBNAME; }

function flagValue(name) {   // --flag=value 形式（预留）
  const hit = argv.slice(1).find((a) => a.startsWith(name + '='));
  return hit ? hit.split('=').slice(1).join('=') : null;
}

/* ==================================================================== */
/* package —— 当前平台 install 包 → out/dist/                           */
/* ==================================================================== */

function cmdPackage() {
  const buildDir = path.join(OUT, 'build-dist');
  const distDir = path.join(OUT, 'dist');
  const isWin = process.platform === 'win32';

  fs.rmSync(buildDir, { recursive: true, force: true });
  fs.rmSync(distDir, { recursive: true, force: true });
  fs.mkdirSync(OUT, { recursive: true });

  const cmakeArgs = [
    '-S', ROOT, '-B', buildDir,
    '-DCMAKE_BUILD_TYPE=Release',
    '-DMP_EMBED_OUTPUT=OFF',
    '-DMP_BUILD_EXAMPLES=ON',
    `-DMP_WITH_TAGLIB=${has('--taglib') ? 'ON' : 'OFF'}`,
    `-DMP_WITH_SYSMEDIA=${has('--sysmedia') ? 'ON' : 'OFF'}`,
    `-DMP_UNIVERSAL_MACOS=${(has('--universal') && process.platform === 'darwin') ? 'ON' : 'OFF'}`,
    `-DCMAKE_INSTALL_PREFIX=${distDir}`,
  ];
  if (isWin) {
    if (has('--taglib')) {
      if (!process.env.VCPKG_ROOT) {
        console.error('✗ --taglib 需要 VCPKG_ROOT 环境变量（vcpkg install taglib:x64-windows）');
        process.exit(1);
      }
      cmakeArgs.push(
        `-DCMAKE_TOOLCHAIN_FILE=${path.join(process.env.VCPKG_ROOT, 'scripts', 'buildsystems', 'vcpkg.cmake')}`);
    }
    cmakeArgs.push('-A', 'x64');
  }

  run('cmake', cmakeArgs);
  run('cmake', ['--build', buildDir, '-j', String(os.cpus().length)]);
  run('cmake', ['--install', buildDir]);

  /* 冒烟 1：纯 C 示例（Windows 多配置生成器下二进制在 Release/ 子目录，跳过） */
  if (!isWin) {
    run(path.join(buildDir, 'mp_smoke'));
  }

  /* 冒烟 2：符号已导出（unix） */
  if (process.platform === 'darwin') {
    const nm = runCapture('nm', ['-gU', path.join(distDir, 'lib', 'libmp_native.dylib')]) || '';
    if (!nm.includes('_mp_version')) { console.error('✗ mp_version not exported'); process.exit(1); }
  } else if (process.platform === 'linux') {
    const nm = runCapture('nm', ['-gD', path.join(distDir, 'lib', 'libmp_native.so')]) || '';
    if (!nm.includes(' T mp_version')) { console.error('✗ mp_version not exported'); process.exit(1); }
  }

  console.log(`\n✓ packaged: ${path.relative(ROOT, distDir)}`);
  walk(distDir).forEach((f) => console.log(`    ${path.relative(distDir, f)}`));
}

/* ==================================================================== */
/* multi —— 多平台产物 → out/multi/<platform-arch>/                      */
/* ==================================================================== */

const CONTAINER_IMAGE = 'ubuntu:24.04';

function hostTag() {
  switch (process.platform) {
    case 'darwin': return 'darwin';
    case 'win32':  return 'win32';
    default:       return 'linux';
  }
}

function cmdMulti() {
  const multiDir = path.join(OUT, 'multi');
  const buildRoot = path.join(OUT, 'build-multi');
  const withDocker = !has('--no-docker');
  const withWin = !has('--no-win');
  const extras = [
    `-DMP_WITH_TAGLIB=${has('--taglib') ? 'ON' : 'OFF'}`,
    `-DMP_WITH_SYSMEDIA=${has('--sysmedia') ? 'ON' : 'OFF'}`,
  ];

  fs.rmSync(multiDir, { recursive: true, force: true });
  fs.rmSync(buildRoot, { recursive: true, force: true });

  const cmakeConfigure = (buildDir, extraArgs = []) => {
    run('cmake', [
      '-S', ROOT, '-B', buildDir,
      '-DCMAKE_BUILD_TYPE=Release',
      '-DMP_EMBED_OUTPUT=OFF',
      ...extras, ...extraArgs,
    ]);
    run('cmake', ['--build', buildDir, '-j', String(os.cpus().length)]);
  };
  const collect = (buildDir, tag) => {
    const dest = path.join(multiDir, tag);
    fs.mkdirSync(dest, { recursive: true });
    fs.copyFileSync(path.join(buildDir, LIBNAME), path.join(dest, LIBNAME));
    return path.join(dest, LIBNAME);
  };

  /* ---- 宿主平台原生构建 ---- */
  if (process.platform === 'darwin') {
    console.log('▶ darwin-arm64（本机原生）');
    cmakeConfigure(path.join(buildRoot, 'darwin-arm64'), ['-DCMAKE_OSX_ARCHITECTURES=arm64']);
    const f1 = collect(path.join(buildRoot, 'darwin-arm64'), 'darwin-arm64');
    run('lipo', ['-info', f1]);

    console.log('▶ darwin-x64（AppleClang 交叉）');
    cmakeConfigure(path.join(buildRoot, 'darwin-x64'), ['-DCMAKE_OSX_ARCHITECTURES=x86_64']);
    const f2 = collect(path.join(buildRoot, 'darwin-x64'), 'darwin-x64');
    run('lipo', ['-info', f2]);
  } else if (process.platform === 'linux') {
    console.log('▶ linux（本机原生）');
    cmakeConfigure(path.join(buildRoot, 'linux-host'));
    collect(path.join(buildRoot, 'linux-host'), `linux-${process.arch === 'arm64' ? 'arm64' : 'x64'}`);
  } else {
    console.log('▷ Windows 宿主：跳过 darwin/linux（Windows 产物经 Docker 或 package 命令产出）');
  }

  /* ---- Docker：linux + windows ---- */
  if (withDocker) {
    let dockerOk = true;
    try { execFileSync('docker', ['info'], { stdio: 'ignore' }); }
    catch { dockerOk = false; }

    if (!dockerOk) {
      console.log('⚠ Docker daemon 未运行，跳过 linux/windows 平台（CI 上可补齐）');
    } else {
      console.log(`▶ linux + windows（Docker：${CONTAINER_IMAGE}）`);
      console.log('  注：docker cp 注入源码，不依赖 bind mount（远程引擎同样可用）');

      const cid = `mp-native-build-${process.pid}`;
      try { execFileSync('docker', ['rm', '-f', cid], { stdio: 'ignore' }); } catch {}

      run('docker', ['run', '-d', '--name', cid, CONTAINER_IMAGE, 'sleep', 'infinity']);
      const cleanup = () => { try { execFileSync('docker', ['rm', '-f', cid], { stdio: 'ignore' }); } catch {} };
      process.on('exit', cleanup);

      console.log('  [docker] 安装工具链（g++ / cmake / mingw-w64 / linux 交叉）...');
      run('docker', ['exec', cid, 'bash', '-c', [
        'set -eu',
        'export DEBIAN_FRONTEND=noninteractive',
        'apt-get update -qq',
        'apt-get install -y -qq --no-install-recommends g++ make cmake file pkg-config libtag1-dev libdbus-1-dev g++-mingw-w64-x86-64 g++-mingw-w64-i686 g++-x86-64-linux-gnu > /dev/null',
      ].join(' && ')]);

      /* 注入源码 */
      execFileSync('docker', ['exec', cid, 'mkdir', '-p',
        '/work/out/multi/linux-x64', '/work/out/multi/win32-x64', '/work/out/multi/win32-ia32']);
      for (const d of ['include', 'src', 'cmake', 'examples', 'scripts', 'tests']) {
        execFileSync('docker', ['cp', path.join(ROOT, d), `${cid}:/work/`]);
      }
      execFileSync('docker', ['cp', path.join(ROOT, 'CMakeLists.txt'), `${cid}:/work/CMakeLists.txt`]);

      console.log('  [docker] 构建 linux-x64 / linux-arm64 / win32-x64 / win32-ia32 ...');
      run('docker', ['exec', cid, 'bash', '/work/scripts/container-build.sh']);

      execFileSync('docker', ['cp', `${cid}:/work/out/multi/.`, `${multiDir}/`]);
      cleanup();
      process.removeListener('exit', cleanup);
    }
  } else {
    console.log('▷ 跳过 Docker 平台（--no-docker）');
  }

  /* ---- 清点 ---- */
  console.log(`\n✓ 多平台产物（${path.relative(ROOT, multiDir)}/）：`);
  const libs = walk(multiDir).filter((f) => /\.(dylib|so|dll)$/.test(f));
  if (libs.length === 0) console.log('  （无）');
  for (const f of libs) {
    const rel = path.relative(multiDir, f);
    const info = runCapture('file', ['-b', f]);
    console.log(`  ${rel.padEnd(44)} ${(info || '').trim().slice(0, 56)}`);
  }
  console.log('\n提示：Stub 模式不含 TagLib/SysMedia——加 --taglib --sysmedia 构建全功能版。');
}

/* ==================================================================== */
/* smoke —— 纯 C 冒烟示例                                               */
/* ==================================================================== */

function cmdSmoke() {
  const buildDir = path.join(OUT, 'build-standalone');
  const smokeExe = path.join(buildDir,
    process.platform === 'win32' ? 'mp_smoke.exe' : 'mp_smoke');

  run('cmake', [
    '-S', ROOT, '-B', buildDir,
    '-DCMAKE_BUILD_TYPE=Release',
    '-DMP_EMBED_OUTPUT=OFF',
    '-DMP_BUILD_EXAMPLES=ON',
    '-DMP_BUILD_TESTS=ON',
  ]);
  run('cmake', ['--build', buildDir, '-j', String(os.cpus().length)]);

  /* core 测试 */
  run('ctest', ['--test-dir', buildDir, '--output-on-failure', '-R', 'core']);
  run(smokeExe, process.argv.slice(2).filter((a) => !a.startsWith('-')));
}

/* ==================================================================== */
/* 工具                                                                  */
/* ==================================================================== */

function walk(dir) {
  const out = [];
  if (!fs.existsSync(dir)) return out;
  for (const e of fs.readdirSync(dir, { withFileTypes: true })) {
    const full = path.join(dir, e.name);
    if (e.isDirectory()) out.push(...walk(full));
    else out.push(full);
  }
  return out;
}

function usage() {
  console.log(`用法: node scripts/build.cjs <command> [flags]

commands:
  package [--taglib] [--sysmedia] [--universal]   当前平台 install 包 → out/dist/
  multi    [--no-docker] [--no-win] [--taglib] [--sysmedia]
                                                  多平台产物 → out/multi/<platform-arch>/
  smoke                                           构建并运行 core 测试 + 纯 C 冒烟示例`);
  process.exit(CMD ? 1 : 0);
}

/* ==================================================================== */

switch (CMD) {
  case 'package': cmdPackage(); break;
  case 'multi':   cmdMulti();   break;
  case 'smoke':   cmdSmoke();   break;
  default: usage();
}
