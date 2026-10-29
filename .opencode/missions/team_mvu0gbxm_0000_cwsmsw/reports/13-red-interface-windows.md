# 13 — Interface Windows : RED textuel attendu observe

Le lead a relu puis execute les cinq controles de texte du helper Windows proposes au commit c32dc75f26fe8f6014d418b50eef3c3c5080a557. Ils lisent le script sans le lancer et ne dependent pas du core d'une autre branche.

Resultat : 5 tests, 2 echecs attendus (parametres explicites VerificationPhase et AllowDependencyResolution absents), 3 succes statiques, code1. Empreintes du test et du helper inchangees avant/apres. Aucun PowerShell, CMake, Conan ou CTest reel n'est execute.

Autorisation GREEN donnee pour cette seule tranche d'interface de configuration, en preservant les usages historiques. Les effets operationnels (absence de bootstrap/reseau, phases isolees, environnement MSVC et codes enfants) ne sont pas prouves par ces tests de texte : leur verification synthetique discriminante reste a proposer avant implementation et acceptance de ces comportements. Aucun parametre present ne sera pris pour preuve d'enforcement runtime.

Auteur : windows-pipeline/build-engineer. Executant : lead, Python3.13.1 Windows. Commande : C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p test_*.py -v.

Preuves brutes et empreintes : out/verification-synthetic/windows-helper-static-red-2026-10-29T23-07-07-159Z/. La tache et le pipeline Windows ne sont ni termines ni integres. Adaptateur en attente d'interfaces communes integrees. Aucun moteur, rendu, benchmark, installation ou cloud.
