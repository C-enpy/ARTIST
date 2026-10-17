# Validation provisoire T13 — pas de PASS global

> Rapport historique de l'auteur, conservé sans réattribution. Clôture après
> intégration/T14 et revues fresh : [bilan final](final-validation.md) et
> [revues indépendantes](final-reviews.md), 2026-10-17. Les gaps natifs restent ouverts.

Date : 2026-10-16. Candidat source :
`89bf66f19bd6ef648b5a7d43f68ce668d7f9d722`, input `1821af6`.
Auteur/exécutant integrate-gpu-qa ; ce rapport **n'est pas une revue indépendante**.
Commandes, environnement, 14 hashes source et logs : `t13-result.md`.

**Exécuté localement : 588/588 tests Node, 0 fail/skip/cancel**, dont 355 harness,
170 producteurs et 63 intégration. Union syntax/schema/offline : 41/81/215 checks,
8 manifests producteurs offline PASS. Tous les missionVerdict restent INCONCLUSIVE.
Ces checks ne sont ni 12 sélections natives ni tests moteur/GPU.

## Critères et registre bidirectionnel

- **G-01 provenance — PASS statique borné / GAP audit futur.** Sources T02/T03
  et provenance opérateur liées ; inputs hashés, aucune copie/exécution externe,
  README préservé. Tests T13 `manual catalog` et `documentation links` vérifient
  références ; pas de nouvelle vérification exhaustive des licences/URLs T02.
- **G-02 différenciation — PASS statique.** 19 responsabilités avec principal,
  skill et nominal/négatif/oracle ; 4 API, 23 IDs profils uniques/17 skills.
  19 tests `domain`, union explicite et 170 contrats producteurs relancés.
  Ces oracles décrivent des procédures, pas 19 fonctionnalités GPU exécutées.
- **G-03 format — PASS local / GAP natif candidat.** 41/81/215 checks et mutants
  malformed-coordinator/collision/missing-link. Découverte historique au HEAD
  82edafe séparée ; aucune nouvelle découverte native sur T13.
- **G-04 routage — GAP / INCONCLUSIVE.** 12 tests de minima et clauses du
  coordinateur PASS **static-presence**. Scénarios natifs exécutés **0/12**,
  exemples opérationnels **0/3** ; plan `native-probe-plan.md` seulement.
  Aucun principal/appel skill réellement observé par T13.
- **G-05 capacités/niveaux — PASS des contrats statiques / GAP exécution.**
  Tests T06–T12 et références différencient compile/offline/API/GPU/image/perf.
  Aucun device/contexte/SDK validé ici ; Metal Windows documentaire seulement.
- **G-06 honnêteté — PASS des contrôles bornés.** Nature des preuves explicite,
  zéro/skip/stale couverts par harness relancé ; mutation T13 native-unavailable
  rend 2, native-unavailable-plus-fail rend 1. JSON de fixture vide ≠ observation.
  L'authenticité du collecteur natif reste GAP ; aucune performance mesurée.
- **G-07 préservation — PASS diff/empreintes / GAP canaris.** Diff source 14
  chemins allowlistés ; 10 sentinelles bytes identiques, 162 lignes non vides de
  l'import opérateur conservées avec repli de ligne permis. Mutants traversal
  et permission élargie refusés par validateur **statique**, pas par le gate natif.
  Aucune écriture moteur/policy/globals ; canaris réels **0**.
- **G-08 idempotence retenue par T03 — PASS local / GAP interruptions natives.**
  Deux runs union offline identiques en bytes sur même candidat. Publication,
  reprise/interruption et state natifs non exercés ; pas de garantie générale.
- **T-01 profils QA — PASS contrats / GAP autonomie.** 58 tests T05, vrais trois
  profils forme B/static-presence, permissions bytes inchangées. Méthode/pins
  déclarés ; aucun comportement QA autonome exécuté par T13.
- **T-02 boucle — PASS raccordement statique / GAP workflow natif.** Clauses
  plan avant auteurs, commit avant worktrees, intégration avant generator,
  reviewer fresh hors board/ré-audit et trois conditions OR explicites.
  Les tests de présence/suppressions ne simulent pas un coordinateur exécuté.
  Politique/pauses/start et grilles inchangés, PM seul paragraphe de séparation.
- **T-03 planner — PASS statique T05 / GAP pratique.** Deux matrices distinctes,
  seuil QA 10/cinq champs, budget/buffer, nominal+négatif et intent rappelés.
  T01 est le plan antérieur, pas une nouvelle exécution du profil natif T13.
- **T-04 generator/healer — GAP après intégration.** Contrats T05 relancés,
  garde TDD auteur et deux réparations conservées ; **T14 non exécuté** par T13.
  Le lead persiste/relit/exécute toute future proposition, bug prod laissé rouge.
- **T-05 reviewer — GAP revue indépendante finale.** Contrat statique T05 relancé ;
  revue R2 préexistante porte T04, pas ce delta. Lead doit encore faire réviser
  T13 avec contexte fresh hors board, puis corriger/ré-auditer si nécessaire.
- **T-06 permissions — blocked / INCONCLUSIVE.** Ruling humain Runner séparé,
  27 règles exactes reconnues. Pas de grant natif par lot, pas d'edit positif,
  pas de canari/gate démontré ; Code Mode fermé empêche ses seuls MCP. Autonomie
  dégradée consentie, **pas un PASS de sandbox/enforcement**. Voie de restitution
  texte et essais exacts à vérifier par le lead, sans élargir les droits.
- **T-07 candidat — PASS local / GAP collecte native.** HEAD, hashes et delta
  vide des entrées validées recoupés au commit source ; logs liés par SHA-256.
  Un merge/diff ultérieur exige revalidation, pas transplantation de ce verdict.

Sens inverse : `final-t04-all.tap` → G-03/06/07/08, T-06/07 (validateur local) ;
`final-producers.tap` → G-02/05, T-01/03/04/05 (contrats déclarés) ;
`final-t13.tap` → G-01/02/03/04/06/07, T-02/06 (présence et mutations CLI) ;
`final-t13-integration-*.json` → G-03/07, T-07 ; `document-check.json` et
`operator-preservation.json` → G-01/07. Tous sous `out/t13-integration/`.
Les logs avant/après sont historiques, pas nouveaux tests distincts.
Aucune preuve orpheline présentée comme native ; aucun GAP ci-dessus reclassé
COVERED par simple nom de test. Les décisions GAP-accept/GAP-defer appartiennent
au lead/utilisateur, sauf le ruling explicite d'autonomie dégradée déjà reçu.

## Top 3 risques QA restants (pas les scores PM)

1. **Permissions : I5 × P4 = 20.** Déclencheur : prendre template B pour gate.
   Blast radius : écritures/commandes hors périmètre QA. Détection : canaris
   jetables, action/règles/trace et sentinelles réelles. Mitigation : deny-all
   inchangé et runner lead distinct, aucun grant. **Gap : T-06 natif blocked.**
2. **Routage : I4 × P4 = 16.** Déclencheur : prendre 12 assertions de texte pour
   12 sélections/charges. Blast radius : mauvaise expertise/API ou contexte saturé.
   Détection : traces principal/skills/références par scénario. Mitigation :
   oracles explicites, clarification et plan lead sous bail. **Gap : G-04 0/12.**
3. **Faux vert : I5 × P3 = 15.** Déclencheur : convertir offline PASS/588 tests
   en résultat GPU ou autonomie. Blast radius : décision d'intégration sans preuve.
   Détection : verdict par niveau/candidat, compte réel, négatifs exit 2/1.
   Mitigation : missionVerdict INCONCLUSIVE, FAIL prioritaire, reviewer fresh.
   **Gap : collecteur natif, QA finale et GPU/image/performance non exercés.**

## Sortie vers le lead

Livrable T13 terminé dans son périmètre local ; intégration/QA finale non déclarées
faites par l'auteur. Recueillir logs privés avant nettoyage du worktree, relire
delta et règles, intégrer puis seulement libérer T14/consommateurs. Ne pas exécuter
les propositions natives sans bail/mandat distinct. QMD reste canonique ; aucune
réaffectation au worktree ni indexation implicite. Actualiser après intégration
canonique autorisée et vérification de la garde de chemin.
