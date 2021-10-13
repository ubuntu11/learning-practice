#!/bin/bash
eval 'set +o history' 2>/dev/null || setopt HIST_IGNORE_SPACE 2>/dev/null
 touch ~/.gitcookies
 chmod 0600 ~/.gitcookies

 git config --global http.cookiefile ~/.gitcookies

 tr , \\t <<\__END__ >>~/.gitcookies
source.developers.google.com,FALSE,/,TRUE,2147483647,o,git-ken.hu.hwacom.com=1//0gB3iRBl49Y3BCgYIARAAGBASNwF-L9Irs_mLZX29i69GMFeibsOcS2XeTFsI6smFxLZXqy2sRpN5xgSOk8jr0hX-gSYHW2l1KeA
__END__
eval 'set -o history' 2>/dev/null || unsetopt HIST_IGNORE_SPACE 2>/dev/null

