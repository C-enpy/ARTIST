# Uniform scenarios — tranche 1, RED_READY

## Entrees et limite de phase

- Base verifiee : `1fba1c98e3a9750f615deacf37a08eb99d4b4fa6`.
- Contrat : `.opencode/evidence/mission-uniform-pipeline/contract.md` ; QA :
  `qa-plan.md` voisin ; brief :
  `.opencode/missions/team_mvu0gbxm_0000_cwsmsw/author-briefs.md`.
- Ce candidat contient **des tests et ce plan seulement**, aucun producteur GREEN.
- Aucun test, compilation, CMake/Conan/CTest, workload CPU/GL, rendu, benchmark,
  installation ou cloud execute par l'auteur. Les checks futurs C++/GL restent
  `NON_EXECUTE` ; aucun RED graphique ni resultat de performance revendique.

## Plan borne et ownership

1. Remettre les tests synthetiques de traduction CLI et lecture du resultat JSON
   (`scripts/tests/verification_scenarios/test_contract.py`). Le lead relit,
   execute RED et autorise la suite avant toute implementation.
2. Apres cette autorisation seulement : implementer le support pur
   `scripts/verification/scenarios/protocol.py`, standard library, sans import
   du modele core, sonde, execution, lecture/ecriture ou initialisation GL.
   Remettre GREEN_READY ; executions GREEN/non-regression reservees au lead.
3. Livrer ensuite les producteurs reels, revus statiquement :
   `ARTIST/verification/CpuUniformWorkload.cpp`, `OpenGLFloatWorkload.cpp`,
   supports locaux et `CMakeLists.txt`. Cette etape n'est pas presente ici et ne
   doit pas preceder la barriere RED de la tranche Python.
4. Transmettre a l'integrateur les descriptions pures et selections CTest par
   noms reels, apres inspection des tests existants. Aucun nom n'est invente dans
   cette premiere remise. Ne pas modifier catalogue core, parents CMake, presets,
   `ARTIST/include/` ni tests existants.

La suite realisera deux routes GL `pass-name` / `prepared-float`, lots 1/16/256,
fixtures source-root `test-datas/shaders/vertex/good/f2-single-values.vert` et
`test-datas/shaders/fragment/good/minimal.frag`. Correctness : 0 -> .5 -> 0,
miroir CPU, readback natif et pixels independants, toutes cellules 32x32.
Preparation/lifetime reutiliseront des selections explicites des tests existants.
La soumission comparera le meme travail, avec chauffe et A/A ; horloge murale
monotone cote CPU, pas temps CPU thread/GPU. Logs, readbacks, instrumentation et
attentes hors chrono ; gardes de production actives. Pas de promesse de gain.

## Interface locale proposee pour la premiere tranche

Cette interface appartient aux scenarios, pas au modele persiste core. Elle ne
recopie pas `Request` ou `ScenarioSpec` et sera composee par l'integrateur.

`build_workload_argv(**selection) -> tuple[str, ...]` recoit les champs scalaires
`executable`, `scenario`, `route`, `profile`, `iterations`, `repetitions`,
`batch_size`, `measurement_requested`, `request_id`, `run_id`, `plan_sha256`,
`result_path`. Il construit uniquement argv, sans lancer le programme.
Le resultat conserve les chemins avec espaces dans un seul argument.
CPU : `cpu.float-mirror`, `pass-name`, `cpu-only`, pas de mesure demandee.
GL : correctness/submission, les deux routes et profils GL explicites.
Preparation/lifetime passent par CTest, jamais par ce constructeur.
Les bornes sont celles du plan QA : iterations 1..1000000, repetitions 1..100,
lot 1/16/256, produit <=100000000 ; entiers stricts (pas bool/float/texte).
Les erreurs de selection sont des `ValueError`, avant tout effet.

`parse_workload_result(text, *, expected) -> dict` lit seulement une chaine JSON.
`expected` contient l'identite et les parametres attendus de l'invocation, pas
un receipt ni un modele core. Le parseur valide le schema v1, les identites,
enums, compteurs non negatifs, coherences de population et samples finis.
Les cles JSON dupliquees et JSON tronques sont rejetes par `ValueError`.
Un PASS avec operations manquantes est rejete. Un FAIL explicite reste une
preuve de domaine lisible, mais ne peut porter une mesure VALID.
Une mesure absente n'est pas zero ; NOT_REQUESTED et ABSENT sont distincts.
Pour la tranche locale, un sample de soumission utilise
`metric=cpu_wall_submission`, `unit=ns`, repetition indexee depuis zero,
`population=iterations*batch_size` ; ce nom n'affirme aucun temps GPU.
Les identites de fichier et renderers sont controles par le core/collecteur,
pas inventes par ce parseur en memoire.

## Runner, attendu RED et preuves manquantes

Depuis le worktree de l'auteur, commande proposee au **lead uniquement** :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p "test_*.py" -v
```

Les imports top-level sont exclusivement standard library ; aucun subprocess,
reseau ou executable natif dans ces tests. Les donnees restent en memoire.
La cible future est chargee par chemin absolu derive du fichier de test, sans
importer le package core ou ses initializers. Aucune fixture persistante creee.

Le module scenario est volontairement absent dans cette remise tests-first.
Chaque test produit alors une assertion explicite `SCENARIO_PROTOCOL_MISSING`,
pas une erreur d'import accidentelle ni un faux module core. Le lead doit
qualifier le RED ; l'auteur ne le declare pas observe. Apres livraison du module,
les assertions exerceraient selection/bornes, argv exact, identites, compteurs,
et mesures avec contre-exemples. Les erreurs d'import du producteur, s'il existe,
ne sont pas capturees ni presentees comme un RED causal.

`model.py`/ContractModel core absent a cette base : vraie dependance pour le
raccordement central, **pas** motif de RED de cette tranche independante.
Pas de copie, import de branche non integree ou contournement de la barriere.
API native C++ de parsing, creation exclusive de fichiers, contexte courant,
FBO/shaders et ordre destruction : futurs controles statiques, pas couverts par
ces tests Python. Revue independante sensible a organiser par le lead avant
reception ; auto-relecture non qualifiee d'independante.
