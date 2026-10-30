# 34 — Source native consolidée en ré-audit

## État de livraison

Le propriétaire a remis les corrections natives de transport Unicode et de cible graphique, puis consigné l'avis levant statiquement le défaut CPU. Le coordinateur a relu le protocole C++, le diff des points d'entrée, les contrôles de contexte/cible et le CMake local. Le ré-audit indépendant de cette implémentation est demandé ; aucune réception globale n'est prononcée avant son résultat.

Les modifications restent locales aux workloads : décodeurs stricts, chemins UTF-8 explicites, entrée wide Windows, manifeste local et garde ACP avant le Reader, paire cible et observations avant uploads/draws. Les blocs de géométrie et de chronométrage déjà revus ne changent pas ; leurs limites sont désormais émises dans les observations.

## Vérification exécutée

La non-régression Python sur ce candidat donne **29/29 tests satisfaits**, aucun error ou skip. Les tests et le protocole Python sont inchangés depuis leur GREEN précédent ; les empreintes des sources contrôlées restent stables pendant le run.

Ce résultat ne valide pas les décodeurs C++, l'incorporation du manifeste, la CRT/STL Windows, les chemins natifs ou OpenGL. Ces éléments restent non compilés et non exécutés conformément au mandat.

## Suite

Attendre le ré-audit du nouveau diff ; retourner au propriétaire tout défaut démontré, puis intégrer par shutdown/merge natifs après acceptation de la source. Les raccordements centraux et la qualification native demeurent distincts. Aucun moteur, build, rendu, benchmark, installation ou cloud lancé.

## Preuves

Candidat `fa828094e67f9ec17eb7b76e38537627d04b0810`, source consolidée `2d212c4`, auteur uniform-scenarios, runner coordinateur. Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p test_*.py -v`.

Logs et empreintes : `out/verification-synthetic/scenario-source-nonregression-2026-10-30T00-33-24-864Z/`. Le dossier auteur `ARTIST/verification/NATIVE_UNICODE_AND_TARGET_READY.md` contient les oracles de lecture et références documentaires ; sa prose n'est pas une preuve d'exécution.
