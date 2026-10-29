# F6 — contrat du template QA déclaré

> Note de portée, 2026-10-29 : le contrat v1 ci-dessous est historique pour les
> trois profils QA. AGENTS.md impose désormais la même politique communication
> v2 au planner, generator et reviewer. Les validateurs reconnaissent exactement
> cette déclaration pour leurs identités/chemins fixes, sans élargir les droits
> des profils ni prouver leur enforcement natif. Voir le plan de raccordement
> `.opencode/evidence/context-publication/qa-plan.md`.
> Cette évolution ne transforme aucune ancienne preuve en validation du v2.

Date : 2026-10-16. Avis engine-architect / qa-policy-boundary, fresh, lecture seule,
PM/LLM C64/R70. Candidat réellement identifié :
`fd1e8cdd6559ec05b26012bb59ed42f05dc73f18`. Aucun test/canari exécuté par l'architecte.
**Décision lead : recommandation retenue**, sans nouvelle permission, grant,
adoption ADR ni changement du ruling humain « Runner séparé ».

## Faits et rectification de brief

Les vrais profils T05 et leur oracle indépendant déclarent **27 triplets** :
DENY-ALL ; read:* ; glob:* ; skill:artist-build-validation ; 16 refus read
`*.env`/`*.env.*` dans les huit casses ; **2** refus récursifs `**/.env*` et
`**/.ENV*` ; refus edit, shell, execute, subagent et external_directory sur `*`.
Le brief du lead annonçait huit refus récursifs : cette quantité était erronée.
Le catalogue résolu peut contenir des règles héritées supplémentaires ; son
comptage n'est pas celui du frontmatter. Aucun profil/oracle ne sera complété
silencieusement pour correspondre au brief ou au nouveau checker.

## Contrat de correction minimal

Conserver la forme A conservatrice actuelle : fichiers exacts non secrets et
IDs skill exacts ; tous ses négatifs F1 non protégés restent rejetés.
Ajouter une forme B **qa-readonly-declared-v1**, reconnaissance de déclaration :

- Identités fixes qa-planner, test-engineer et test-reviewer, mode subagent,
  chemins exacts `.opencode/agents/<id>.md`. Le candidat, son manifest ou son
  texte ne peut déclarer un nouveau rôle autorisé ou sélectionner une exemption.
- Tableau de confiance fixé dans le code du harness, versionné et relu, avec
  exactement les 27 triplets ordonnés ci-dessus. Comparaison structurelle des
  valeurs littérales sensibles à la casse ; aucune extension par inclusion de
  quelques gardes, simulation glob ou normalisation des règles. Les variantes
  de format ne sont acceptées que si le parseur strict produit les mêmes valeurs.
- Ne pas charger arbitrairement le template depuis le profil, manifest ou
  expectations.json candidat. L'oracle T05 reste indépendant et inchangé.
- Aucun ajout/suppression/permutation/doublon, `ask` remplaçant `deny`, wildcard
  substitué, réouverture tardive, nouveau skill/tool/grep/MCP/team_* autorisé.
- Échec de B ne permet pas un fallback A qui admettrait un wildcard large.
  PERMISSION_SUBAGENT, identité, schéma, collisions et autres FAIL restent actifs.
- Sortie : nom/version du template, agent/path, premier écart de triplet,
  `evidenceKind: static-presence`, libellé « template déclaré reconnu ».
  Jamais « isolation/enforcement vérifié ». D-PERM = **mode décidé, enforcement
  non prouvé**. missionVerdict/T-06 natif restent INCONCLUSIVE/blocked ; données
  native-evidence non authentifiées restent exit2, ou exit1 avec autre FAIL.

Les 27 règles sont la déclaration réellement choisie en T05, pas une certification
de confidentialité exhaustive : chemins/jonctions, secrets arbitraires, règles
globales/session, casse effective et actions MCP restent à éprouver nativement.
Le runner de confiance reste humainement mandaté : tests relus, exécution et
persistance par le lead, attribution réelle. Aucun runner autonome n'est créé.

## RED/GREEN attendu et refus à préserver

RED causal sur les trois consommateurs réels T05 inchangés : actuellement exit1
PERMISSION_QA. Après correction T04 seule : reconnaissance B de présence, aucune
certification native, les trois sujets effectivement contrôlés et missionVerdict
toujours INCONCLUSIVE. Variantes de format supportées et forme A restent nominales.

Mutants valides de schéma, refusés pour la bonne cause : chaque garde retirée,
déplacée avant read:*, changée en ask/allow ; règle tardive read:* ou read:**,
secret exact .ENV/.eNv.local rouvert ; external_directory supprimé/assoupli ;
execute/shell/edit/subagent/MCP/team_* ask/allow ; skill:* ; grep:* ; doublon/
permutation ; mode primary/all, ID/chemin ou rôle usurpé. Ne pas fabriquer un RED
par parse malformé ni changer l'oracle T05 pour faire passer T04.

Une alternative « fichiers exacts seulement pour tous » laisserait F6 ouvert et
changerait la lecture utile du ruling ; un évaluateur natif maison est hors
contrat. Aucun échec de canari manquant n'est accepté implicitement. T13 attend
la correction intégrée et sa vérification ; ré-audit F1–F5 en parallèle indépendant.
