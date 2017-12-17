#!/bin/sh

SERVER_PORT=1234
SERVER_HOST=::1

HASH_EXAMPLE=8NmV2C3vdRN5K2ShsdU8TZUaG04frUFTB7w1mQYndD1TT6gY88NfbhTchRTwiCYAMHH2PJKK

# quitte le serveur
quit_server() {
  info "on quitte le serveur si jamais il était déjà lancé..."
  nc -q1 -w1 -u "$SERVER_HOST" "$SERVER_PORT" << EOF
S
EOF
}

# affiche une information
info () {
  echo " \033[34m[I]\033[0m\t \033[1m$1\033[0m"
}

# affiche une erreur sans quitter
## par exemple les pertes de paquets on les signale, mais on ne bloque pas
err () {
  echo " \033[31m[ERR]\033[0m\t \033[1m$1\033[0m"
}

# permet d'afficher un message d'erreur et de stopper
fail () {
  err "$1"
  exit 1
}

# Un peu de nettoyage avant de commencer...
quit_server
rm -f tests.log

info "TESTS DES ARGUMENTS..."
./server 2> tests.log >&2 && fail "Pas d'args pour le serveur"
./client 2> tests.log >&2 && fail "Pas d'args pour le clients"
./server "$SERVER_HOST" 2> tests.log >&2 && fail "Manque le port d'écoute"
./client "$SERVER_HOST" "$SERVER_PORT" put minihash 2> tests.log >&2 && fail "Hash trop petit"
./client "$SERVER_HOST" "$SERVER_PORT" wtf "$HASH_EXAMPLE" 2> tests.log >&2 && fail "Action inconnue (sans IP)"
./client "$SERVER_HOST" "$SERVER_PORT" wtf "$HASH_EXAMPLE" ::1 2> tests.log >&2 && fail "Action inconnue (avec IP)"

info "TESTS DES LANCEMENTS..."
./server "$SERVER_HOST" "$SERVER_PORT"& 2> tests.log >&2 || fail "Le serveur n'a pas pu démarrer"; sleep .2
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE" ::42 & 2> tests.log >&2 || fail "Le client 1 n'a pas pu se lancer"; sleep .1
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE" localhost & 2> tests.log >&2 || fail "Le client 2 n'a pas pu se lancer"; sleep .1
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE" ::1 & 2> tests.log >&2 || fail "Le client 3 n'a pas pu se lancer"; sleep .1
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE"1 ::42 & 2> tests.log >&2 || fail "Le client 4 n'a pas pu se lancer"; sleep .1
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE"2 ::432 & 2> tests.log >&2 || fail "Le client 5 n'a pas pu se lancer"; sleep .1
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE"2 ::42 & 2> tests.log >&2 || fail "Le client 6 n'a pas pu se lancer"; sleep .1
./client "$SERVER_HOST" "$SERVER_PORT" put "$HASH_EXAMPLE"2 ::432 & 2> tests.log >&2 || fail "Le client 7 n'a pas pu se lancer"; sleep .1
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE" > /dev/null || fail "Le client 8 n'a pas pu se lancer"; sleep .1

# on attend un peu pour éviter de faire les tests suivants avant que tout soit correctement envoyé
sleep .2

info "TESTS DES RÉSULTATS ATTENDUS..."
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE"2 | grep "::432" > /dev/null || err "perte d'une IP (::432)"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE" | grep "::1" > /dev/null || err "perte d'une IP (::1)"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE" | grep "::42" > /dev/null || err "perte d'une IP (::42)"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE"1 | grep "::42" > /dev/null || err "perte d'une IP (::42)"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE"2 | grep "::42" > /dev/null || err "perte d'une IP (::42)"

sleep .2

info "TESTS DU TTL... (30sec; le temps de boire quelques gorgées de café :p)"
sleep 30
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE"2 | grep "::432" > /dev/null && fail "soucis 1 avec le TTL !"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE" | grep "::1" > /dev/null && fail "soucis 2 avec le TTL !"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE" | grep "::42" > /dev/null && fail "soucis 3 avec le TTL !"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE"1 | grep "::42" > /dev/null && fail "soucis 4 avec le TTL !"
./client "$SERVER_HOST" "$SERVER_PORT" get "$HASH_EXAMPLE"2 | grep "::42" > /dev/null && fail "soucis 5 avec le TTL !"

# on laisse le temps de bien tout recevoir avant de stopper le serveur
sleep .2
quit_server

# on nettoie
rm -f tests.log
info "TOUS LES TESTS SONT PASSÉS ! =)"
exit 0
