# 14 — Modeles : GREEN observe, coeur complet encore en cours

La tranche Request/CommandSpec de pool-3 est verifiee par le runner lead au candidat dd20115a8ea40cba9867183a416149bb080c3c9b : 28 tests sur 28, aucune erreur/echec/skip, code0. Le fichier de tests est inchange depuis le RED e88d070 ; le diff modifie le modele et la documentation de remise uniquement. Empreintes stables pendant le run.

La source relue applique les bornes et types stricts, copie les sequences, conserve env_delta dans une copie immuable et retourne des exports detaches. Aucun processus, reseau ou workload n'est utilise par cette tranche.

Preuves brutes : out/verification-synthetic/core-model-green-2026-10-29T23-09-03-404Z/ ; RED correspondant : core-model-red-2026-10-29T23-01-29-624Z/. Commande : C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p test_*.py -v ; auteur pool-3, executant lead, Windows/Python3.13.1.

Le lead autorise maintenant la proposition de tests de la tranche catalogue/planification pure et des interfaces restantes, avant leur implementation. Cette acceptation est bornee aux modeles : le coeur n'est ni termine ni integre, et les adaptateurs ne recoivent pas implicitement une branche isolee. Les vraies dependances d'interface restent signalees.

Le prochain groupe devra verifier absence d'effets et compatibilite des scenarios, sans lancer outils de build, tests moteur ou programmes GL. Le protocole de compteurs du producteur scenarios doit etre compose plutot que redefini avec un schema incompatible.

Aucun fonctionnement global Windows/Linux, rendu ou performance n'est demontre par ce GREEN synthetique.
