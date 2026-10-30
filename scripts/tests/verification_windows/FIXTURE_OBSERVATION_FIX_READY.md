# FIXTURE_OBSERVATION_FIX_READY — oracle parent, produit inchange

**Remise historique V1 :** son accord getter type/snapshot comme garde a ete
refute par le selfcheck lead. Utiliser `FIXTURE_OBSERVATION_V2_READY.md` : le
snapshot est l'oracle, les deux formes de getter restent diagnostiques seulement.

## Preuve avant correction

Diagnostic readonly lead, logs existants relus via Context Mode :

- `out/verification-synthetic/windows-env-readonly-2026-10-30T03-58-25-449Z/` :
  getter assigne initialement NULL, puis typedNull=False/typedEmpty=True,
  cle presente/vide dans snapshot ; fonction non appelee, garde initiale en refus.
- `out/verification-synthetic/windows-env-readonly-snapshot-gate-2026-10-30T03-59-10-257Z/` :
  accord getter type + snapshot avant, DANS et apres la fonction pure ; cle
  presente/vide, relecture assignee non nulle apres setup. Le NULL initial est
  garde comme observation, jamais normalise en chaine vide.

Conclusion bornee : l'ancien getter assigne ne prouve pas une suppression ENV.
Le mecanisme host/effet de setup n'est pas attribue plus loin. Autorisation lead :
remplacer l'observation defectueuse dans native_fixture.ps1, pas modifier le
produit ENV, l'invariant parent ou ses assertions. Auteur : aucun run.

## Diff seul et mapping de conservation de l'oracle

Fichier executable modifie uniquement : `native_fixture.ps1`.

- **Avant** : les deux maps parent etaient remplies par le getter assigne
  `[Environment]::GetEnvironmentVariable(name, 'Process')`, montre instable.
- **Apres** : un snapshot neuf `[Environment]::GetEnvironmentVariables(
  [EnvironmentVariableTarget]::Process)` par stage, lu uniquement par Contains
  et get_Item sur les QUATRE noms fixes KEEP/EMPTY/DROP/ONLY prefixed ARTIST_NATIVE_.
  Aucune enumeration/serialization des autres noms ou valeurs du dictionnaire.
- **Meme forme** : parent_before et parent_after restent des maps de ces quatre
  cles, valeur string exacte si presente (y compris ''), null si absente.
  Aucun cast [string], normalisation, heritage, env restore, PInvoke ou nouvelle
  dependance. fixture_version reste 1 ; pas de schema core cree ou recopie.
- **Meme assertion metier** : `assertEqual(report['parent_before'],
  report['parent_after'])` demeure INCHANGEE. La comparaison Python du vrai
  environnement parent avant/apres demeure aussi INCHANGEE.

Exemples de conservation :

- present-vide -> absent donne '' -> null : comparaison toujours en echec ;
- valeur A -> valeur B reste A -> B : comparaison toujours en echec ;
- absent -> present-vide donne null -> '' : comparaison toujours en echec ;
- present-vide inchange reste '' -> '' : vrai succes, sans convertir le NULL
  d'un ancien lecteur en vide. La valeur vient du snapshot, pas d'un fallback.

## Caracterisation readonly et refus des contradictions

Inline dans la fixture, sans nouvelle fonction PS/pipeline/processus :

1. Conserver la lecture assignee initiale des seuls quatre noms pour diagnostic.
2. Creer le delegate du seul getter public exact (string, EnvironmentVariableTarget)
   -> string, puis prendre le snapshot Process. Ne charger/evaluer aucune fonction
   native avant que chaque marqueur ait satisfait l'accord ci-dessous.
3. Pour chaque nom, exiger : valeur snapshot presente => type string, nullness
   identique entre getter type/snapshot, egalite a vide identique, valeur egale
   selon StringComparer.Ordinal. Un desaccord est HARNESS/code96 avant appel.
   Verifier aussi les marqueurs initiaux exacts deja injectes par run_native :
   KEEP=parent, EMPTY='', DROP=parent-only et ONLY absent. Un etat initial
   incorrect ne doit pas devenir un faux succes par simple egalite avant/apres.
4. Effectuer le meme controle apres cleanup, sur un nouveau snapshot. Desaccord
   => HARNESS/code96, aucune comparaison permissive ou succes silencieux.

La lecture assignee instable n'est PAS gate : le diagnostic lead a precisement
refute cet oracle. Son desaccord n'est cependant pas efface : champ diagnostique
additionnel parent_observation.before/after, par marqueur seulement, avec
assigned_null/length/matches_snapshot, present, snapshot_null/empty et
typed_null/empty/matches_snapshot. Le NULL initial reste trace, pas transforme.
Le booleen sanitized_initial explique un refus de setup sans publier d'autre env.
L'accord requis est celui du getter type et du snapshot, deja caracterise par
le lead en scope caller/callee/caller. Aucune autre variable d'environnement
n'est publiee et aucune nouvelle assertion de resultat n'est inventee.

Le controle precede aussi le refus de pin deliberement teste : aucune source
non epinglee n'est evaluee. Avant/apres restent comparables quand test14 presente
une source modifiee pour obtenir le code96 attendu. Une impossibilite de lire
l'observateur est une panne HARNESS avec result.json conserve, pas RED produit.

## Inchanges et risques restants

Les cinq substitutions de getters relues (3 produit, 2 fixture) sont inchangees.
Produit, pin natif, tests15/anciens37 et assertions, enfant, protocole de ce
dernier, autres fixtures/audits inchanges.
Pas de nouveaux Process/PSI/Start/Kill, delais, streams, CLI/helper complet,
installation, moteur, MSVC/vcvars, GL ou cloud dans le correctif.

Test06 reparse reste NON_EXECUTE. La capture et les getters produit n'ont pas
de nouveau GREEN ; Dispose lecteur avant Wait face aux writers herites reste
une limite de reclamation, non levee par cet oracle ou son pin.

## Proposition lead apres revue du diff

Cwd :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-windows-pipeline/scripts/tests/verification_windows`

Les 14 noms explicites de NATIVE_GREEN.md (test14 premier, test06 exclu) restent
la selection a reutiliser via Context Mode, sans discovery globale. Conserver
logs bruts/identites et examiner parent_observation en cas de refus HARNESS.
Puis les 37 anterieurs, sans modification :

```powershell
C:/Python313/python.exe -B -m unittest -v test_psbase_audit test_targeted_contracts test_wrappers test_dispatch test_helper_contract
```

Prevision NON OBSERVEE : 14/14 + 37/37, zero failure/error/skip si les controles
et fonctions satisfont leurs contrats. Aucun nouveau resultat GREEN affirme.
Ce correctif donne un observateur parent caracterise, pas une dispense de relire
les nouvelles erreurs causales de capture ou des autres fonctions si elles arrivent.

Matrice auteur : analyse des traces EXISTANTES + syntaxe/AST/diff statiques
seulement ; fixture corrigee, controle readonly et suites : NON_EXECUTE.
