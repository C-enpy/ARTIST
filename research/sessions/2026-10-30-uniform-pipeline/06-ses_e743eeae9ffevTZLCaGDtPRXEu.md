# Planifier les raccordements restants

Session: ses_e743eeae9ffevTZLCaGDtPRXEu
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T07:42:20.988Z — msg_18bc27b39001NVD7M7XUMUginm

## Delta du plan restant — à valider par le lead

**Mission existante conservée, sans nouvelle admission ni exécution.** Politique native lue : QA proportionnée, livraison pour qualification ultérieure, tests synthétiques seulement. Aucun barème numérique spécifique supplémentaire dans `model-policy.json` ; évaluations **PM/LLM**, plancher développement appliqué.

### État et limites vérifiés

- Rapports **112/113** : Linux intégré ; Windows adapté mais helper fonctionnel encore fermé. Les résultats historiques restent attribués à leurs candidats, pas aux futurs changements.
- `build-windows.ps1:517–522` ferme effectivement l’accès ; `524–576` contient le dispatch prototype, dont une branche `run`.
- L’oracle `WindowsRealHelperBarrierTests.test_real_helper_remains_closed_before_dispatch` vérifie cette fermeture : **ne pas simplement le supprimer pour obtenir GREEN**.
- Les deux workloads existent dans `ARTIST/verification/CMakeLists.txt`. Le raccordement aux parents manque.
- HEAD `a7aacb9…` fourni par le lead, **non vérifié par Git dans cette consultation** ; aucun état propre ni absence de diff local présumé.
- AGENTS, principes via `artist-wiki`, contrat, bindings, plan QA et politique consultés. Aucun symbole moteur exploré, fichier modifié ou test exécuté.

## Conditions communes

- Auteurs : **tests proposés avant code**, aucun run personnel. Le lead relit le confinement, observe le RED causal, puis autorise GREEN et exécute les vérifications synthétiques.
- Aucun helper complet, CMake configure, téléchargement, build, CTest, moteur CPU/GL ou benchmark exécuté.
- Tout nouveau fragment PowerShell exécuté doit être extrait explicitement, relu et composé uniquement avec feuilles factices autorisées.
- Tous les contextes proposés : **`fresh`**, brief autonome ; aucune reprise d’historique nécessaire.
- Les estimations ci-dessous sont des scores de routage, pas des durées garanties ni la matrice QA I×P.

## Fiches restantes

### Q — Addendum QA A+B déjà en consultation

- **Objectif/livrable :** obtenir la proposition pool-21, puis faire persister/committer par le lead les obligations A+B, mappings d’oracles et limites d’exécution.
- **Type / profil conseillé :** planification QA, `qa-planner`, lecture seule.
- **Entrées / propriété :** contrat, bindings, rapports112/113, présent delta ; aucun fichier mutable possédé par le planner.
- **Contrat :** QA proportionnée ; pas de nouvelle campagne, pas d’écriture de tests à la place des auteurs. Les risques QA ≥10 gardent déclencheur, portée, détection, mitigation et gap.
- **Vérification :** lecture et restitution textuelle runtime ; contrôle lead de cohérence et commit avant worktrees.
- **Dépendances / exclusivité :** aucune nouvelle production parallèle de cet addendum ; réutiliser la consultation déjà ouverte.
- **Contexte / worktree :** `fresh`, `false` ; consultation autonome.
- **Évaluation :** PM/LLM, brut **46**, retenu **46**, risque **45**. Incertitude : proposition actuelle non reçue dans cette consultation.

### A — Helper Windows prepare/build fonctionnel, borné

- **Objectif/livrable :** raccorder les entrées et feuilles privées nécessaires à **prepare/build seulement**, avec tests de fragments et preuve synthétique WindowsAdapter→factory commune.
- **Type / profil conseillé :** développement script/harness, `build-engineer`.
- **Entrées / chemins possédés :** HEAD intégrant Q ; `scripts/build-windows.ps1` et tests/fixtures concernés sous `scripts/tests/verification_windows/`. Adaptateur en lecture, sauf transfert explicite d’ownership si un défaut y est démontré.
- **Contrat / binding times :**
  - Avant effets : flags, payload privé, phase/configuration/cibles/chemins admis.
  - À prepare : bootstrap explicitement autorisé, Conan2 avant configure, aucun build/test.
  - À build : arbre préparé et cibles explicites ; aucun bootstrap/configure réparateur.
  - Pendant les enfants : environnement outil maintenu ; restauration exacte sur succès, refus, exception et code enfant non nul.
  - Run direct reste au runner commun ; **helper-run non supporté par l’adaptateur**.
  - Legacy sans nouveau mode préservé. Régions **WRAPPERS/NATIVE et pins source-only inchangés par défaut**.
- **Critères / vérification lead :**
  1. Mapping relu **ancienne fermeture → nouvelles gardes → tests discriminants**, avant modification de l’oracle.
  2. RED des fragments : entrées invalides sans effet, opt-in absent, séparation prepare/build, absence de fallback legacy/run, espaces/arguments, code enfant, restauration absent/vide.
  3. GREEN avec doubles stricts ; chaque feuille réelle interdite échoue immédiatement si atteinte par le harness.
  4. Test effectif de composition adaptateur/factory, gap signalé au rapport113.
  5. Non-régression Windows puis core/contexte/scénarios selon sélection relue.
  
  Commande candidate : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_windows -p 'test_*.py' -v`, **uniquement après audit des tests réellement sélectionnés**.
- **Dépendances :** Q committé ; aucun besoin d’attendre B.
- **Ressources exclusives :** fichier helper, fixtures et sorties synthétiques A ; pas d’arbre build.
- **Contexte / worktree :** `fresh`, `true` ; isolation de l’auteur, RED/GREEN restant dans cette tranche.
- **Évaluation :** PM/LLM, brut **78**, retenu **78**, risque **68**. Incertitudes : mapping précis des feuilles de réemploi legacy et lifetime du processus/environnement.

### B — Raccordement CMake et workflow manuel

- **Objectif/livrable :** rendre sélectionnables les cibles existantes et livrer le workflow manuel borné prévu au contrat.
- **Type / profil conseillé :** développement configuration/harness, `build-engineer`.
- **Entrées / chemins possédés :** HEAD intégrant Q ; `CMakeLists.txt`, `ARTIST/CMakeLists.txt`, `.github/workflows/verification-pipeline.yml`, nouveau dossier dédié de tests statiques d’intégration. Workloads, presets et helpers en lecture.
- **Contrat :** `ARTIST_BUILD_VERIFICATION=OFF` par défaut ; inclusion conditionnelle de `verification` après disponibilité de `ArtistLib`, indépendante de `BUILD_TESTING`. Aucun ajout automatique CTest ni modification des campagnes existantes. Workflow exclusivement manuel, entrées bornées, aucune interpolation shell libre, installation ou campagne implicite.
- **Critères / vérification lead :** tests statiques RED avant modification : option/default, garde et chemin d’inclusion, deux cibles exactes, absence d’auto-exécution ; contrôle du déclenchement manuel et des commandes du workflow. GREEN par lecture structurée/fixtures ou commandes intégralement factices. **Aucun configure réel**, même pour vérifier OFF.
  
  Commande candidate, après création effective : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_integration -p 'test_*.py' -v`.
- **Dépendances :** Q committé ; **pas A**, interfaces et noms déjà fixés.
- **Ressources exclusives :** parents CMake, workflow, sorties synthétiques B.
- **Contexte / worktree :** `fresh`, `true` ; fichiers distincts de A.
- **Évaluation :** PM/LLM, brut **53**, retenu **70**, risque **44**. Incertitude : détails du workflow à maintenir strictement dans le contrat existant.

### R — Revue indépendante du helper après GREEN

- **Objectif/livrable :** avis ciblé sur entrée→dispatch→feuilles, réemploi legacy et lifetime environnement/processus ; findings localisés et conditions de fermeture.
- **Type / profil conseillé :** revue indépendante, `correctness-reviewer`.
- **Entrées / propriété :** commit/diff exact A, mapping, tests et preuves lead ; aucune édition.
- **Contrat :** vérifier également les chemins d’échec et l’absence de seconde capture/helper-run ; pas d’élargissement aux régions source-only sans nécessité démontrée.
- **Critères / commandes :** revue statique et rapprochement des preuves synthétiques ; aucune exécution requise du reviewer. Finding bloquant corrigé par A puis ré-audité.
- **Dépendances :** A GREEN ; indépendante de B.
- **Ressources exclusives :** aucune en lecture ; candidat figé pendant l’avis.
- **Contexte / worktree :** `fresh`, `false` ; indépendance requise.
- **Évaluation :** PM/LLM, brut **64**, retenu **64**, risque **62**. Incertitude : preuves synthétiques ne qualifient pas MSVC natif.

### G — Generator des seuls gaps démontrés — différé

- **Objectif/livrable :** matrice obligations/preuves restante et propositions de tests discriminants, sans doublonner le RED des auteurs.
- **Type / profil conseillé :** génération QA consultative, `test-engineer`.
- **Entrées / propriété :** A+B réellement intégrés, avis R et preuves attribuées ; **remise textuelle ou patch proposé**, aucune édition/exécution par ce profil restrictif.
- **Contrat :** pas de production modifiée ; les auteurs appliquent les propositions retenues dans leur ownership et suivent RED/GREEN avec runner lead.
- **Critères / commandes :** chaque proposition cite un gap observable et son contre-exemple ; aucun test sans obligation. Commandes proposées seulement, relues par le lead.
- **Dépendances :** barrière I ci-dessous, pas seulement fin des workers.
- **Ressources exclusives :** aucune ; une éventuelle correction réouvre A ou B, sans second écrivain.
- **Contexte / worktree :** `fresh`, `false`.
- **Évaluation :** PM/LLM, brut **55**, retenu **70** si production de code de test proposé, risque **47**. Incertitude : peut conclure « aucun gap additionnel démontré ».

### D — Photographie documentaire finale — différée

- **Objectif/livrable :** `docs/development/verification-pipeline.md` et références strictement nécessaires : commandes livrées, phases, limites, attribution et qualification future.
- **Type / profil conseillé :** documentation, `build-engineer` avec mandat documentaire borné.
- **Entrées / chemins possédés :** A+B intégrés et preuves disponibles ; documentation uniquement, aucune modification du contrat ratifié ou du plan QA.
- **Contrat :** « livré pour qualification », jamais fonctionnement Windows/Linux natif, rendu ou performance démontrés. Distinguer tests Python sur Windows, fragments PS factices et sources C++ non compilées.
- **Critères / commandes :** contrôle liens, noms/flags/cibles contre sources ; aucune exécution des exemples. Statuts ADR inchangés.
- **Dépendances :** I ; peut avancer parallèlement à G, puis ajustement si corrections.
- **Ressources exclusives :** fichiers documentaires attribués.
- **Contexte / worktree :** `fresh`, `true`.
- **Évaluation :** PM/LLM, brut **35**, retenu **35**, risque **30**. Incertitude : photographie à figer après corrections intégrées.

### J — Audit final fresh — différé, hors board d’implémentation

- **Objectif/livrable :** verdict indépendant PASS/FAIL/INCONCLUSIVE limité à la livraison et aux preuves synthétiques.
- **Type / profil conseillé :** audit QA, `test-reviewer`.
- **Entrées / propriété :** candidat final intégré, G traité explicitement, D intégrée, preuves et avis R ; lecture seule.
- **Contrat :** audit des obligations modifiées, pas réouverture systématique des fondations ni campagne inventée. Aucun test écrit/exécuté par l’auditeur.
- **Critères / commandes :** correspondance candidat/preuves, raccordements présents, oracles non affaiblis, gaps/skips/NON_EXECUTE visibles ; corrections propriétaires et ré-audit ciblé si nécessaire.
- **Dépendances :** intégration des corrections retenues de G et D finale.
- **Ressources exclusives :** candidat figé ; aucune ressource native.
- **Contexte / worktree :** `fresh`, `false`, indépendance obligatoire.
- **Évaluation :** PM/LLM, brut **58**, retenu **58**, risque **53**. Incertitude : pas de verdict natif possible sous ce mandat.

## Barrières du coordinateur et ordre prêt

1. **I0 :** recevoir Q, ratifier/persister/committer ; vérifier HEAD et fichiers disponibles **avant création des worktrees A/B**.
2. **A ∥ B** ; runner lead sérialisé si sorties ou environnement partagés. Aucun build/indexeur à lancer.
3. **R après A GREEN**, pendant B si encore en cours.
4. **I :** intégrer les commits revus A et B, contrôler le diff réel et lancer les seules non-régressions synthétiques relues. Aucun consommateur ne lit implicitement un diff non committé.
5. **G ∥ D** après I ; corrections retenues par les propriétaires, intégrées avant **J**.

**Chemin critique probable :** Q → A → R → I → gaps/corrections éventuels → J. B peut devenir critique si son raccordement n’est pas prêt à I.

## Admission et décisions restantes

- Ces références sont **locales**, pas des IDs Ensemble. Aucun outil d’équipe appelé, aucune tâche publiée.
- Affinités proposées : `agent` indiqué par fiche, `context: fresh`, `worktree` explicite, dépendances ci-dessus. **Schéma d’import effectif non inspecté sous l’interdiction de teamtools** : le lead doit vérifier leur conservation native avant admission ; ne pas importer un payload qui les perd.
- Conserver la tâche parent Windows ouverte jusqu’au raccordement intégré. Les anciennes productions remplacées restent inactives avec remplacement explicite ; ne pas les marquer completed sans livrable ni permettre un redispatch concurrent.
- Mapping des feuilles Windows : décision technique locale à documenter avant code. **Si son traitement exige un nouveau contrat public d’environnement/processus ou une modification des régions pins/source-only, arrêter cette extension et transmettre au lead/architecte**, sans la résoudre ici.
- Aucun nouveau consentement utilisateur requis pour les étapes courantes déjà déléguées ; aucun élargissement du mandat ou de ses interdictions proposé.