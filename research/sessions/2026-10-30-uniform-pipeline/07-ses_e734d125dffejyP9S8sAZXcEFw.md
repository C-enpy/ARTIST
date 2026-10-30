# Planifier les compléments QA

Session: ses_e734d125dffejyP9S8sAZXcEFw
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T12:06:14.100Z — msg_18cb41350001o82DAAS5PYmHOl

## Plan proposé au coordinateur — aucune publication

**Deux branches indépendantes**, production en lecture seule pour tous les auteurs QA. Seuls les tests/fixtures changent ; seul le lead exécute les programmes synthétiques préalablement relus.

### Prérequis communs — barrière H0, coordinateur

Avant les worktrees :

- Vérifier le HEAD/diff annoncé : `52e7330` helper, `9aedc0a` collecteur intégrés ; distinguer les commits documentaires `6c51046` et `9a45bf6`. Ces identités sont fournies, **non vérifiées ici par Git**.
- Persister/committer l’addendum G1–G4 au plan QA §13/14, avec ownerships et mapping préalable.
- Fournir localement les rapports 142–144 et le candidat textuel `result_mvuxb9xc_008c_y6ti6z`, sans supposer leur accès par les profils restrictifs.
- Fixer un HEAD **H0 contenant toutes ces entrées** ; contrôler leur présence dans chaque worktree.
- Vérifier le profil auteur : édition des seuls tests autorisée, processus interdits. Le `test-engineer` restrictif déjà observé ne doit pas recevoir une demande de patch incompatible avec ses permissions.

La politique persistée de `team_mvu0gbxm_0000_cwsmsw` a été lue : mandat synthétique/qualification confirmé, aucun barème spécifique complexité/risque trouvé. Évaluation **PM/LLM**, sans sélection de modèle ni modification de politique.

## A — Couture G1 : plan local, puis tests dédiés

### A0 — Proposition de fixture avant code

- **Type / profil conseillé :** investigation bornée ; expertise tests Python/PowerShell et intégrité des preuves.
- **Livrable :** plan textuel de la couture, points d’extraction réels, passage des données et liste des tests ; aucun patch.
- **Entrées / chemins possédés :** H0, contrats helper/binding, QA W3–W8, sources de tests existantes ; aucun fichier mutable possédé.
- **Contrat :** expliquer comment les fragments PowerShell admis, avec factory mémoire, produisent réellement le témoin du **même bundle** consommé par `evidence.prepare`, empreinté avant receipt puis réadmis par `build`, sans recréation des métadonnées.
- **Acceptation :** identifier explicitement pourquoi `NativeBindingCase.run_case` ne convient pas tel quel ; proposer le support minimal **test-owned**, ses sentinelles et son confinement. Aucun nouveau contrat/API production.
- **Vérification :** revue statique lead du plan et des points d’extraction ; aucune commande auteur.
- **Dépendances :** H0 ; A1 attend validation explicite de cette proposition.
- **Ressources :** aucune exécution ni bundle partagé.
- **Contexte :** `fresh`, brief autonome ; `worktree: false`.
- **Estimation PM/LLM :** brute **63**, retenue **63**, risque **59**. Difficulté : preuve de continuité et doubles trompeurs ; incertitude : support PS minimal encore à ratifier.

### A1 — Réalisation G1, après validation A0

- **Type / profil conseillé :** développement de tests/harness ; auteur habilité à éditer sans exécuter.
- **Livrable / ownership :** nouveaux tests et fixtures dédiés G1, noms exacts réservés à la validation A0. **Ne modifie aucun fichier B**, ni les fixtures historiques communes.
- **Entrées :** H0 + proposition A0 validée et persistée.
- **Contrat / critères :**
  - Nominal : témoin réellement émis, mêmes octets, hash inscrit avant receipt, build réadmet sans réécriture.
  - Mutation/absence/refus : arrêt avant adaptateur/process.
  - Échec Conan : partiels conservés, première cause préservée, aucun témoin/receipt favorable fabriqué.
  - Interdits : `witness_data` favorable préconstruit, bundle copié, faux receipt, juxtaposition de deux tests indépendants.
- **Vérification :** proposer au lead la sélection exacte `python -B -m unittest …` après fixation des noms ; lead seul observe résultats et contre-exemples.
- **TDD :** caractérisation si production correcte ; oracle invalidé par mutations ciblées. Aucun faux RED imposé. Une régression production révélée reste un constat, pas une autorisation de correction.
- **Dépendances :** H0 et validation A0.
- **Ressources :** bundle propre à chaque cas ; exécution PS sérialisée par le lead.
- **Contexte :** `fresh` avec plan validé local ; `worktree: true`.
- **Estimation PM/LLM :** brute **78**, retenue **78**, risque **73**. Couture interlangage et attribution des preuves ; prérequis principal : A0 accepté.

## B — G2 + migration G3/G4, une tranche cohérente

- **Type / profil conseillé :** développement de tests Python/PowerShell ; auteur habilité à éditer sans runner.
- **Livrable / ownership :** nouveaux tests d’environnement effectif Windows ; `test_wrappers.py`, `test_dispatch.py`, `dispatch_fixture.ps1` et métadonnées historiques **explicitement nommées dans le mapping validé**. Aucun fichier A ; aucun changement production/pins.
- **Entrées :** H0, contrats ratifiés, propositions G2–G4.
- **Avant édition :** remettre le mapping bidirectionnel **ancienne obligation/assertion ↔ preuve actuelle/test de remplacement**, y compris chaque adaptation de métadonnée fake.
- **Contrat / critères :**
  - **G2 :** observer l’environnement effectivement transmis à la factory Popen via `WindowsAdapter → SubprocessRunner`, pas seulement `env_delta`. Parent intégralement synthétique ; six champs dérivés indépendamment du plan, identifiants liés ; forcing logiciel retiré pour hardware, contexte hardware ambiant retiré pour CPU ; héritage restant et parent inchangés. Refus Windows software conservé.
  - **G3 :** remplacer l’attente d’une barrière globale inexistante par les admissions fines prepare/build et refus run/flags **avant payload/outils** ; préserver legacy et pins.
  - **G4 :** migrer le prototype historique : prepare Conan avant configure, build sans bootstrap, run argv directs, codes/restauration/sentinelles. Pas de helper-run réintroduit, global skip, archive invisible ou obligation supprimée sans remplacement.
- **Vérification :** commandes proposées au lead pour les nouveaux tests et `test_wrappers.py`/`test_dispatch.py`, puis découverte Windows relue. Aucun lancement auteur.
- **TDD :** caractérisation et contre-exemples des oracles ; échec d’un vieux prototype ≠ RED causal contre la production.
- **Dépendances :** H0 uniquement ; mapping approuvé avant première édition.
- **Ressources :** fixtures/bundles uniques ; PS et faux programmes exécutés exclusivement et séquentiellement par le lead.
- **Contexte :** `fresh`, brief autonome ; `worktree: true`.
- **Estimation PM/LLM :** brute **73**, retenue **73**, risque **66**. Environnement effectif et conservation d’obligations historiques couplés ; incertitude : liste exacte des métadonnées touchées.

## Intégration et QA indépendante

**I — Barrière coordinateur, pas simple fin auteur :** relire les patches, intégrer A1 et B, vérifier absence de changement production/pins, exécuter les sélections synthétiques autorisées, puis enregistrer HEAD/diff, commandes, comptages, skips et limites. Les consommateurs attendent ce candidat intégré.

**Q — Audit final hors board d’implémentation :**

- **Profil conseillé :** `test-reviewer`, lecture seule, indépendant ; ne rédige ni ne répare les tests.
- **Livrable :** mapping G1–G4/W3–W8, verdicts PASS/FAIL/INCONCLUSIVE sur preuves intégrées.
- **Entrées / ownership :** candidat I, patches, mappings et résultats lead ; aucun fichier mutable.
- **Vérification :** lecture croisée des assertions, fixtures et traces ; aucun processus demandé.
- **Dépendance :** I réellement intégré.
- **Ressources :** aucune exclusive.
- **Contexte :** `fresh` ; `worktree: false`.
- **Estimation PM/LLM :** brute **61**, retenue **61**, risque **60** : risque résiduel de fausse preuve favorable, distinct de la matrice QA I×P.

### Remise native et disposition des anciennes tâches

Le schéma découvert accepte `agent`, `context`, `worktree`, `depends_on`, `complexity`, `risk`, `report_mode` et `completion_policy`. Utiliser `report_mode: "final_response"` pour les profils restrictifs. Les affinités machine des auteurs restent **à vérifier dans le catalogue** : ne pas importer sous un profil générique faute de profil compatible. Les références H0/A0/A1/B/I/Q sont locales ; le lead remplacera les dépendances par les vrais IDs.

Conserver inactive/différée toute ancienne tâche remplacée ; ne pas la marquer completed sans livrable intégré et ne pas laisser son ancien ID produire un doublon. La proposition textuelle pool37 reste une entrée, pas un patch livré.

**Parallèle :** A0 puis A1 d’un côté ; B de l’autre, sans dépendance A↔B.  
**Chemin critique probable :** H0 → A0 → validation → A1 → I → Q ; B rejoint I.  
**Exclusions :** runs PS et fixtures partagées sérialisés par le lead.  
**Décisions restantes :** fixture minimale A0, mapping exact B, profils auteurs effectivement autorisés. Aucun changement d’architecture ou de statut ADR proposé.

**Limites de lecture :** contrats locaux lus ; QMD `artist-wiki` consulté, sans retrouver le guide récent par son chemin. Aucune exécution, édition, publication ou sous-délégation effectuée.