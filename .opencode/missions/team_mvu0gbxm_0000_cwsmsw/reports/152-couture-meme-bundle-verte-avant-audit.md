# 152 — Couture du même bundle verte, audit indépendant à préparer

La correction d'une ligne du harnais a été relue : quatre noms internes du parseur
piné sont ajoutés, sans changement des opérateurs, gardes, assertions ou production.
Le blob reçu correspond au candidat. Après merge natif et commit, le coordinateur
a exécuté d'abord les six baselines, puis les quatre caractérisations.

**6/6 baselines et 4/4 caractérisations passent**, sans échec, erreur ou skip,
avec empreintes stables. Les erreurs du candidat précédent restent documentées
au rapport150 ; elles ne sont pas rebaptisées RED de production.

Le nominal observe le même bundle, les vrais reçus et la conservation des octets,
hashes et mtime du payload et du témoin entre prepare et build. Les deux négatifs
refusent mutation et absence du témoin avant description build/runner/factory sans
réparation. Conan17 conserve les partiels collectés avant reçu défavorable et la
première cause malgré une anomalie secondaire de collecte.

Les livrables G1 et Windows sont désormais intégrés et vérifiés localement. Les
tâches d'auteur peuvent être closes sur ces livrables réels ; la réception globale
reste ouverte jusqu'à l'audit de tests fresh, hors board d'implémentation, puis
l'actualisation documentaire. L'admission du profil restrictif sera vérifiée avant
cet audit. Aucun nouveau jalon d'approbation utilisateur n'est nécessaire.

## Preuves et attribution

- Candidat réparation pool-44 `be18e95ead1f2be4fa87e42cd73d7c57f8e38653` ; un seul
  fichier, une ligne, Git blob249822496962618eeff3a298a0657474ad06d2ed.
- Intégration testée `165aee5f1a1da7db8431555d196081f802558ed2` ; auteur couture
  pool-41, auteur réparation pool-44, persistance couture et runner coordinateur.
- Baselines : `out/verification-synthetic/windows-seam-baseline-v2-2026-10-30T12-50-42-192Z/`.
- Caractérisations : `out/verification-synthetic/windows-seam-characterizations-2026-10-30T12-51-14-668Z/`.
- Commandes Python3.13 `-B -m unittest` sur G1BaselineTests puis
  G1CharacterizationTests du module scripts.tests.verification_seams.test_g1_windows_helper_seam,
  option `-v`, cwd racine ARTIST. Manifestes : arguments exacts, hashes et journaux ;
  stdout pointe les arbres originaux conservés, pas des bundles copiés.

Le rôle python-pip est simulé comme les autres outils ; seule l'entrée composée
admise et le driver test-owned sont exécutés dans PowerShell. Aucun helper complet,
pip/Conan/CMake/CTest réel, cmd/vcvars, moteur même CPU, OpenGL, rendu, benchmark,
installation ou cloud exécuté. La qualification native Windows/Linux/GPU reste
NON_EXECUTE. Les19 tests Windows du rapport149 gardent leur preuve distincte.
