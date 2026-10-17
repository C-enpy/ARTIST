# Livraison partielle GPU/QA — bilan final

Date : 2026-10-17. Coordinateur/exécutant des revalidations : lead.
Tranche finale documentaire locale : PM/LLM C35/R40 brut/retenu ; aucun code
exécutable nouveau, contrôles adaptés sans faux RED comportemental.
Candidat de revue : `245f1f30e7a668619a12399b0334473ce4a1c225`, branche isolée
`mission/gpu-qa-integration-e52f73a`. Aucune fusion vers main autorisée ou effectuée
pour cette clôture. Le commit final ajoute seulement documents et preuves ; le
snapshot local de validation doit donner son HEAD/delta et ses hashes exacts.

## Résultat et attribution

- **Livré localement** : 23 profils, 17 skills, 19 responsabilités/quatre API,
  12 oracles de choix, catalogue manuel, validateur gradué, boucle QA raccordée
  au coordinateur existant, runner séparé. OpenGL reste seul backend du moteur.
- **PASS local recoupé** : 588 tests Node distincts, zéro échec/skip/cancel ;
  union syntax/schema/offline 41/81/215 contrôles, pas des tests supplémentaires.
  Commandes/logs/hashes dans t13-integration.md ; sources/logs re-hashés par le
  reviewer. Snapshot exact 245f1f3 offline : 215 PASS, delta vide.
- **PASS borné de revue** : final-qa-review et final-gpu-mapping fresh,
  indépendants et parallèles, aucun finding démontré ni patch demandé.
  Rapports attribués dans final-reviews.md ; aucun test exécuté par les reviewers.
- T14 a appliqué la méthode generator avec un substitut build-engineer, zéro
  nouveau test proposé, KEEP des preuves utiles/DEFER natif. Revue QA finale
  via correctness-reviewer substitut ; vrais profils absents au root d'admission,
  présents dans le worktree. **Ni ces substitutions ni leur transport ne prouvent
  le lancement ou l'enforcement des vrais profils QA.**
- Base opérateur : six fichiers importés séparément et identiques en 82edafe.
  Readme.md : 53 dépôts avec auteurs/organisations et liens, commit cb91076 ;
  sources consultées distinctes des imports/dépendances, notices non remplacées.

Les résultats auteur T13 dans validation-report.md restent historiques. RED
auteurs (dont correctifs R/R2), GREEN et limites sont conservés sans les attribuer
aux reviewers ou au generator. Rien ici n'est une nouvelle baseline C++/OpenGL.

## Contrôles de clôture réellement terminés

Après redémarrage du serveur, l'appel interrompu n'avait persisté aucun résultat
final exploitable, et aucun processus de test n'était encore actif. Pas un RED
du harness. Les trois logs de la campagne 588 ont été recomptés/re-hashés à leurs
empreintes t13-integration.md, sans rejouer les 355/170 tests non touchés.
Seuls les contrôles affectés par la documentation finale ont été relancés :

- `node --test --test-concurrency=1 --test-reporter=tap
  .opencode/tests/agentic/t13-integration/contract.test.mjs` : **63/63**, zéro
  fail/skip/cancel, exit0. Ce sont 63 des 588, pas 63 tests distincts ajoutés.
- Union CLI syntax/schema/offline : **41/81/215**, exit0, PASS locaux et
  missionVerdict INCONCLUSIVE. JSON avec identité/hashes du diff conservés.
- `git diff --check` exit0, Readme.md identique à cb91076, journal append-only,
  six wikilinks de la synthèse résolus ; chemins moteur/build/politique/config
  exclus inchangés par rapport à la base d'intégration 79596ca.

Exécutant : lead. Logs : `out/agentic/mission-gpu-qa/final-local/`.
`t13-after-docs.tap` SHA256 :
`73cdbc58b352243bdf4b48b77633f48048748a056fd4817a4d4eafae4950dc7b`.
`summary.json` SHA256 :
`7d199c436d777293f9f7f6fc84f97d01fb9bc6e4c3f98c751d12ba1a182668d9`.
Ces sorties ne revendiquent aucune exécution native/GPU supplémentaire.

## Registre de sortie

**G-01/02** : provenance et différenciation statiques PASS bornés, pas nouvel audit
exhaustif de toutes les licences. **G-03** : syntax/schema/offline PASS ; découverte
native du candidat constatée par GET, pas comportement. **G-04** : **0/12** choix
natifs, **0/3** exemples opérationnels, NOT_RUN. **G-05** : niveaux/oracles définis,
aucune preuve API/image/perf exécutée. **G-06/07** : honnêteté/diff préservé et
diagnostics locaux vérifiés ; canaris/gates natifs **0**, donc INCONCLUSIVE.
**G-08/T-07** : identité/idempotence locale contrôlées ; interruptions/publication/
collecte native non exercées. **T-01…03** : contrats et boucle déclarés vérifiés,
pas workflow natif autonome. **T-04/T-05** : étapes méthodologiques réalisées
après intégration, attribution/fallback explicites, pas vraie admission QA native.
**T-06** : blocked/INCONCLUSIVE pour enforcement et restitution native, ruling
humain runner séparé conservé ; aucun grant par lot vérifié.

Découverte actuelle : **30 profils résolus (23 locaux + 7 natifs)**, 17 skills
locaux présents parmi 103 résolus. Permissions QA : **27 règles déclarées / 32
résolues** héritage inclus ; aucune de ces quantités ne prouve un refus natif.
Pas d'exécution CPU/GPU mesurée, C++/CTest/CI distante non relancés pour ce lot.

## Reprise obligatoire avant toute revendication native/globale

Propriétaire : lead/utilisateur, puis spécialiste adapté si critère demandé.
Pas de nouveau worker ou permission créé implicitement par ce bilan.

1. Obtenir un contexte d'admission autorisé pour les vrais profils, sans copier
   sur main pour contourner la restriction, sans élargir les droits/transport.
2. Exécuter S01–S12 et OP1–OP3 avec traces principal/appels/références du candidat.
3. Vérifier positifs/négatifs QA, actions/règles exactes, sentinelles et refus
   natifs ; l'absence d'un profil n'est pas un refus du gate.
4. Tester séparément workflow autonome/interruptions/publication/collecteur.
   API/GPU/image/perf demandent leurs preuves propres, pas un lookup ou un mock.

Plan : native-probe-plan.md ; risque/obligation/oracle et propriétaire : qa-plan.md,
t14-proposal.md et final-reviews.md. **Aucun GAP-accept humain implicite**, aucun
PASS global. Refus de blanchir les gaps, pas refus de livrer les documents utiles.

## Documentation, publication et limites

Synthèse wiki mise à jour dans docs/agentic/gpu-qa-harness.md et journal append-only.
ADR et code moteur/build/politique/globals inchangés par la mission. QMD artist-wiki
reste attaché au vault canonique : aucun rebind ni réindexation du worktree ;
rafraîchissement après intégration canonique explicitement autorisée et garde
de chemin vérifiée. Push demandé par l'utilisateur, uniquement la branche isolée,
sans force ; hash distant à recouper avant annoncer la publication.

Logs privés préservés sous out/agentic/mission-gpu-qa/ (auteurs R2/T13 et lead).
Pas de nettoyage/purge de branches ou de worktrees dans cette clôture ; ces logs
ignorés ne sont pas publiés par Git. Rapports/hashes/provenance versionnés permettent
leur attribution, pas leur reconstruction ni une preuve native manquante.

## Sondes natives du 2026-10-17

Le lead a exécuté le routage et le gate sur OpenCode v2.0.30, candidat
`3f7d9c42d97835a2e0d4348337886a13b831eb41`, copie jetable seulement.
Détail, sessions et écarts : native-routing-enforcement.md.

- **G-04** : S01–S12 ont émis le principal attendu et les skills minimum.
  S09 a exigé une reprise parce que la première collecte avait interrompu
  l'enfant avant les skills. N02, API non précisée, a choisi OpenGL : échec du
  négatif. OP1–OP3 restent non exécutés.
- **T-06** : l'évaluateur et les outils `read`/`skill` appliquent les 32 règles
  résolues. Le positif inoffensif et le skill autorisé passent ; `.env`,
  chemin absolu externe et skill non autorisé sont refusés. **Échec** : un lien
  symbolique interne a livré le contenu externe `INERT_EXTERNAL`. Les routes
  opérateur `session.shell` et `session.skill` ne passent pas par ce gate.
- GPU, image, performance et grant d'édition restent non prouvés. Pas de PASS global.
