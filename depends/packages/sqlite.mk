package=sqlite
$(package)_version=3220000
$(package)_sha256_hash=2824ab1238b706bc66127320afbdffb096361130e23291f26928a027b885c612
$(package)_file_name=$(package)-autoconf-$($(package)_version).tar.gz
$(package)_download_path=https://www.sqlite.org/2018/
$(package)_patches=sqlite.patch

define $(package)_preprocess_cmds
   patch -p1 < $($(package)_patch_dir)/sqlite.patch
endef

define $(package)_set_vars
  $(package)_config_opts=--disable-readline
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef

define $(package)_postprocess_cmds
endef
