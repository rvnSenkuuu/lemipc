#!/usr/bin/env bash
set -e

HEADER_FILE="incs/lemipc.h"

BOARD_WIDTH=$(grep -E "^#define[[:space:]]+BOARD_WIDTH[[:space:]]+[0-9]+" "$HEADER_FILE" | awk '{print $3}')
BOARD_HEIGHT=$(grep -E "^#define[[:space:]]+BOARD_HEIGHT[[:space:]]+[0-9]+" "$HEADER_FILE" | awk '{print $3}')
MIN_PLAYER=3
MAX_PLAYERS=$((BOARD_HEIGHT * BOARD_WIDTH))

echo "Dimensions de la carte récupérées : $BOARD_WIDTH x $BOARD_HEIGHT ($MAX_PLAYERS cases max)"

read -rp "Nombre de joueurs : " NUM_PLAYERS
if ! [[ "$NUM_PLAYERS" =~ ^[0-9]+$ ]]; then echo "Entrée invalide"; exit 1; fi
if (( NUM_PLAYERS < MIN_PLAYER )); then echo "Au moins $MIN_PLAYER joueurs requis"; exit 1; fi
if (( NUM_PLAYERS > MAX_PLAYERS )); then echo "Trop de joueurs pour la map ($MAX_PLAYERS max)"; exit 1; fi

read -rp "Nombre d'équipes : " NUM_TEAMS
if ! [[ "$NUM_TEAMS" =~ ^[0-9]+$ ]]; then echo "Entrée invalide"; exit 1; fi
if (( NUM_TEAMS < 1 || NUM_TEAMS > NUM_PLAYERS )); then echo "Nombre d'équipes invalide"; exit 1; fi

read -rp "Mode texte (y/n) [défaut: y] ? " yn
if [[ "$yn" == "n" || "$yn" == "N" ]]; then
  TEXT_MODE=""
  echo "Mode graphique activé."
else
  TEXT_MODE="--text"
  echo "Mode texte activé."
fi

PIDS=()

TEAM_ID=1
if [[ -x "./lemipc" ]]; then
  ./lemipc "$TEAM_ID" "$NUM_PLAYERS" &
  PID_CREATOR=$!
  echo "Créateur (Joueur 1, Équipe $TEAM_ID, PID $PID_CREATOR) lancé. Initialisation de la mémoire..."
else
  echo "./lemipc introuvable"; exit 1
fi

sleep 1

kill -STOP "$PID_CREATOR"
PIDS+=( "$PID_CREATOR" )
echo "Créateur suspendu."

for i in $(seq 2 "$NUM_PLAYERS"); do
  TEAM_ID=$(( (i-1) % NUM_TEAMS + 1 ))
  
  if [[ -x "./lemipc" ]]; then
    ./lemipc "$TEAM_ID" "$NUM_PLAYERS" &
    pid=$!
    kill -STOP "$pid"
    PIDS+=( "$pid" )
    echo "Joueur $i lancé (PID $pid) -> équipe $TEAM_ID (suspendu)"
  else
    echo "./lemipc introuvable"; exit 1
  fi
done

sleep 0.2

if [[ -x "./glemipc" ]]; then
  ./glemipc $NUM_TEAMS $TEXT_MODE &
  VISU_PID=$!
  echo "Visualiseur lancé (PID $VISU_PID). Préparation de l'affichage..."
else
  echo "./glemipc introuvable"
  for pid in "${PIDS[@]}"; do kill "$pid" 2>/dev/null || true; done
  exit 1
fi

sleep 1

for pid in "${PIDS[@]}"; do
  kill -CONT "$pid" 2>/dev/null || true
done
echo "Tous les joueurs ont été réveillés. Le jeu commence !"

wait "$VISU_PID" 2>/dev/null
echo "Le visualiseur a été fermé."

for pid in "${PIDS[@]:-}"; do
  kill "$pid" 2>/dev/null || true
  wait "$pid" 2>/dev/null || true
done