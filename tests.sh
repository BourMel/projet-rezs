#!/bin/sh

SERVER_PORT=1234
SERVER_HOST=::1

HASH_EXAMPLE=8NmV2C3vdRN5K2ShsdU8TZUaG04frUFTB7w1mQYndD1TT6gY88NfbhTchRTwiCYAMHH2PJKK


quit_server() {
  info "on quitte le serveur"
  nc -w1 -u "$SERVER_HOST" "$SERVER_PORT" << EOF
S
EOF
}

info () {
  echo " \033[34m[I]\033[0m\t $1"
}

fail () {
  echo " \033[31m[ERR]\033[0m\t $1"
  exit 1
}

quit_server

rm -f tests.log

info "TESTS DES ARGUMENTS"
./server 2> tests.log >&2 && fail "Pas d'args pour le serveur"
./client 2> tests.log >&2 && fail "Pas d'args pour le clients"
./server "$SERVER_HOST" 2> tests.log >&2 && fail "Manque le port d'écoute"
./client "$SERVER_HOST" "$SERVER_PORT" wtf "$HASH_EXAMPLE" 2> tests.log >&2 && fail "Action inconnue (sans IP)"
./client "$SERVER_HOST" "$SERVER_PORT" wtf "$HASH_EXAMPLE" ::1 2> tests.log >&2 && fail "Action inconnue (avec IP)"


info "TESTS DE LANCEMENT"
./server "$SERVER_HOST" "$SERVER_PORT"& 2> tests.log >&2 || fail "Le serveur n'a pas pu démarrer"
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE" ::42 & 2> tests.log >&2 || fail "Le client 1 n'a pas pu se lancer"
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE" ::1 & 2> tests.log >&2 || fail "Le client 2 n'a pas pu se lancer"
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE" ::1 & 2> tests.log >&2 || fail "Le client 3 n'a pas pu se lancer"
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE"1 ::42 & 2> tests.log >&2 || fail "Le client 4 n'a pas pu se lancer"
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE"2 ::432 & 2> tests.log >&2 || fail "Le client 5 n'a pas pu se lancer"
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE"2 ::42 & 2> tests.log >&2 || fail "Le client 6 n'a pas pu se lancer"
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE"2 ::432 & 2> tests.log >&2 || fail "Le client 7 n'a pas pu se lancer"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE" & 2> tests.log >&2 || fail "Le client 8 n'a pas pu se lancer"

info "TESTS DE RÉSULTATS ATTENDUS"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE" | grep "::1" || fail "perte d'une IP (::1)"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE" | grep "::42" || fail "perte d'une IP (::42)"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE"1 | grep "::42" || fail "perte d'une IP (::42)"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE"2 | grep "::42" || fail "perte d'une IP (::42)"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE"2 | grep "::432" || fail "perte d'une IP (::432)"

sleep .2
quit_server


rm -f tests.log
info "TOUS LES TESTS SONT PASSÉS !"
