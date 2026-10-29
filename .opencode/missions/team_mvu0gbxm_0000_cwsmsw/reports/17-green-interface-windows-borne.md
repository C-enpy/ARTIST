# 17 — Interface Windows verte ; comportement encore a livrer

Le lead a relu le diff du candidat 01e970b682a90d32e1be71ad997475c9d206bf2c : parametres VerificationPhase prepare/build/run et AllowDependencyResolution ajoutes. Toute phase de verification explicite est temporairement refusee avant le chemin historique, plutot que de retomber sur la campagne implicite. Les assertions existantes restent inchangees.

Run lead : 5 controles de texte sur 5 satisfaits, code0, empreintes stables. Aucun PowerShell ni outil de construction reel execute. Preuve : out/verification-synthetic/windows-helper-static-green-2026-10-29T23-15-35-041Z/. Auteur windows-pipeline ; executant lead, Python3.13.1 Windows. Commande : C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p test_*.py -v.

Ce GREEN ne termine pas l'adaptateur Windows. Les nouvelles phases sont volontairement indisponibles dans ce candidat intermediaire. Le lead demande maintenant les tests comportementaux du dispatch sous doubles/harnais factices, a relire avant tout lancement et avant implementation operationnelle : consentement dependances, commandes par phase, codes enfants, restauration d'environnement et arguments avec espaces.

Les interfaces communes non integrees restent une dependance reelle pour l'adaptateur Python ; les controles independants peuvent continuer sans copier les schemas. Aucune preuve reseau/MSVC, compilation, moteur, image ou performance n'est revendiquee par cette tranche.
