# Revue fresh T04-R2 — décision d'intégration

Date : 2026-10-16. correctness-reviewer / review-r2-contract, contexte fresh,
PM/LLM C74/R80. Candidat propre avant/après :
`13b0977480cb993af2fe2dbd7e3ebc1360506c68`. Diff 4024337..13b0977 relu.
Lecture seule : aucun test/canari/API modèle/GPU, écriture, sous-agent ou mutation
MCP/index. Les exécutions recoupées sont celles du lead et de l'auteur.

## Résultat

**Aucun finding démontré, aucune question bloquante dans ce périmètre.**

- F6 : 27 triplets littéraux ordonnés correspondent à l'oracle T05 indépendant.
  Autorité fixée dans le validateur, pas chargée du candidat ; ID/path/mode
  gardés, copie sous ID étranger refusée, aucun wildcard admis via fallback A.
  PERMISSION_SUBAGENT conservé ; nom/version/agent/path/premier écart et
  static-presence conformes au contrat adopté.
- F1-R : suffixe .env suivi de fin ou point refusé sans distinction de casse
  dans chaque composant ; production.ENV.local ne satisfait plus A. Anciens
  wildcards/secrets refusés, fichiers exacts non sensibles conservés.
- F2-R : types core/décimaux traités avant grammaire plain positive ; ,item et
  fractions signées ne deviennent plus strings, contreparties quotées gardées.
  Sous-ensemble explicitement documenté, pas promesse YAML universelle.
- Verdict : missionVerdict reste INCONCLUSIVE ; native sans authenticité exit2,
  FAIL prioritaire exit1. Aucune certification native ou sandbox introduite.

Régressions contract.test.mjs:501-670 : vrais T05, format équivalent, mutations
des 27 règles, ouvertures tardives, ordre/casse, usurpations et fausse autorité
candidat. Les principales mutations gardent parse/schéma PASS avant le refus de
permission attendu ; elles ne fabriquent pas un RED de parse. Nominaux A/core/
quoted conservés, aucune assertion/oracle affaibli trouvé. Profils/ensemble T05
et runner/helper/isolation Git inchangés contre 4024337.

## Preuves recoupées et limites

Hashes source et logs exacts des rapports auteur/lead recoupés : **355/355**
harness + **170/170** contrats locaux, zéro fail/skip/cancel ; trois vrais QA
forme B/static-presence ; huit manifests PASS offline, chaque missionVerdict
INCONCLUSIVE. RED auteur 252 cas = 34 PASS + 218 FAIL, pas 218 vulnérabilités.
Les logs RED/GREEN préservés ne sont pas annoncés comme réexécutés par le reviewer.

PASS borné de la revue statique F6/F1-R/F2-R. Natif, canaris/enforcement, sélection
des scénarios, GPU/image/performance restent INCONCLUSIVE/non exercés. Aucune
preuve exhaustive ; F3–F5 étaient déjà contre-revus et non modifiés dans R2.

Décision du coordinateur après recoupement : lever le blocage de code de T13.
Ses vrais producteurs sont intégrés, les 525 tests et huit manifests ont été
relancés par le lead. T13 peut modifier ses seuls fichiers partagés dans un
worktree seedé sur le HEAD intégré, pas sur main ni sur une branche d'auteur.
L'autorisation ne change ni la politique modèle ni le ruling du runner séparé,
et ne constitue pas une levée de pause opérateur ou un PASS global.
