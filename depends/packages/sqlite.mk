package=sqlite
$(package)_WEBSITE=https://www.sqlite.org/
$(package)_DESCR=SQLite
$(package)_IGNORE=
$(package)_VERSION=3210000
$(package)_sha256_hash=d7dd516775005ad87a57f428b6f86afd206cb341722927f104d3f0cf65fbbbe3
$(package)_SUBDIR=$(package)-autoconf-$($(package)_VERSION)
$(package)_FILE=$(package)-autoconf-$($(package)_VERSION).tar.gz
$(package)_URL=https://www.sqlite.org/2017/$($(package)_FILE)
$(package)_DEPS=cc

define $(package)_preprocess_cmds
  ./autogen.sh
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
