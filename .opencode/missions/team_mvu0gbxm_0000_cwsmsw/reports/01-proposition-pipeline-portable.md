# 1 — Livrer le pipeline, reporter la campagne

## Objectif et stade

Livrer un banc de verification des uniforms et parametres GPU prepare pour Windows et Linux. Les machines maitrisees, les campagnes et les mesures viendront ensuite. Execution en pause, politique non approuvee ; aucun worker Ensemble admis ni changement de source dans cette preparation.

Le titre natif « Performance testing on uniform and GPU parameters » est precise par les criteres du brouillon : livraison de l'outil, sans obligation de gain ni de tests moteur verts.

## Constats reutilises

- Les lecteurs memorisent deja les emplacements OpenGL. Comparer les acces par nom et prepares mesure plusieurs differences internes, pas uniquement une recherche pilote.
- Le test PreparedFloatUniformRenderTests.cpp contient deja une reference pixel independante pour chaque route ; ses readbacks et diagnostics doivent rester hors chronometrage.
- Windows dispose d'un preset Release ; Linux n'en dispose pas dans les presets actuels et son helper impose Debug.
- linux-evidence.py exige llvmpipe et reconcilie dynamiquement decouverte et resultats : les nombres historiques ne sont pas codes en dur.
- Le wrapper Windows initialise puis restaure l'environnement MSVC. Cet environnement doit etre conserve pendant les futures commandes de construction.
- Ni la compilation ni le fonctionnement du futur pipeline ne sont prouves ici.

Sources locales : CMakePresets.json ; scripts/build-windows.ps1 ; scripts/build-linux.py ; scripts/linux-evidence.py ; docs/development/linux-qa.md ; docs/adr/README.md ; ADR-0004 et ADR-0006. Statuts ADR inchanges.

## Apercu des lots proposes, non publies

1. **Contrat et scenarios** : interfaces, formats, profils et plan QA cible avant auteurs. Architecture et QA en lecture seule pour preparer ; persistance par le lead seulement dans la phase autorisee.
2. **Orchestration et preuves** : Python commun, modes consultatifs sans effets, erreurs/delais, identites et rapports. Ownership propose scripts/verification et scripts/verify.py.
3. **Adaptateur Windows** : reemploi du wrapper, environnement MSVC et separation bootstrap/build/run ; ownership Windows exclusif.
4. **Adaptateur Linux** : Release explicite et profils logiciel/materiel sans perte des garanties llvmpipe ; ownership Linux et presets exclusif.
5. **Scenarios float** : reference visuelle, refus/revocation, controle du travail, preparation et mesures CPU de soumission. Code test-only ; pas de modification du moteur par defaut. Google Benchmark reste un candidat optionnel, pas une dependance adoptee.
6. **Integration et entree manuelle** : consommateurs seulement apres integration des producteurs ; workflow manuel distinct et documentation, sans location cloud ni campagne automatique.
7. **Reception independante** : revue des risques de provenance, erreurs et mesure ; exigences QA du projet conservees, pas remplacees par le seul niveau generique de la politique.

Graphe : contrat integre puis coeur, Windows, Linux et scenarios en branches independantes ; integration avant packaging ; audit sur candidat integre. Un ecrivain par fichier, aucun build ou contexte GPU concurrent partage. Aucun consommateur n'est publie sur la seule fin de travail de son producteur.

Le PM a estime les lots de developpement a complexite retenue 70–78 et risque 58–71, selon PM/LLM (estimations, pas probabilites). Ces estimations restent au plan ; aucune classification ni substitution de modele n'est demandee. Le catalogue natif propose conserve sa politique par defaut.

## Politique effectivement proposee

Version 2 ; providers openai, opencode, opencode-go et zai-coding-plan ; seuil initial de planification 50 ; classification manuelle non activee automatiquement ; aucune delegation long_run. Catalogue et prix indicatifs inchanges. Chaque tache garde sa session et son modele ; aucun modele impose aux spawns.

Validation proposee : proportionnee. Les criteres metier ont ete ajoutes a verification.acceptance dans model-policy.json ; le hash presente precedemment n'est donc plus le hash de cette revision. Une nouvelle presentation native par config est requise avant toute approbation start.

## Decision necessaire avant implementation

Le projet exige un test en echec observe avant implementation puis sa reussite, y compris pour les scripts du pipeline. Reporter absolument toute execution empecherait cette preuve. Une revue statique ne la remplace pas.

**Recommandation :** autoriser seulement les validations du pipeline lui-meme sur donnees synthetiques et petits processus factices, sans lancer les tests moteur, le rendu ni les benchmarks. Les campagnes reelles restent reservees aux machines maitrisees futures. Les commandes exactes seront arretees et presentees dans la politique avant demarrage ; aucune n'est autorisee par ce rapport.

**Alternative :** reporter aussi ces validations. La livraison peut alors rester documentaire et statique ; l'implementation soumise au TDD attend son environnement autorise. Aucun PASS runtime, ni RED/GREEN, ne sera invente.

Le niveau proportionne est recommande ; il ne supprime pas le plan QA et les revues obligatoires du projet. Aucune nouvelle infrastructure n'est necessaire pour discuter ce choix. Couts et delais cloud non determines ; aucune ressource reservee.

## Limites et controles effectues

Lectures et recherches documentaires seulement, plus modifications du brouillon de politique et de ce rapport. Aucun build, test ou benchmark execute ; aucune politique approuvee. Les permissions QA observees sur disque ne prouvent pas leur application au runtime : admission effective a verifier avant tout futur worker.
