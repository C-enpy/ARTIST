# Native Unicode/target — SOURCE_READY pour reviewdiff, NON_EXECUTE

## Autorisation et attribution

Le lead annonce GREEN hardware Python sur `82e6cc849de2fee9555c745863d9e0bf9e226e9b` :
29/29, 0 error/skip, tests inchanges, hashs stables. Journaux annonces :
`out/verification-synthetic/scenario-hardware-green-2026-10-30T00-10-40-796Z/`.
Le lead autorise ensuite la paire native, les gardes de contexte et le contrat
Unicode propose dans SOURCE_REVIEW_FIX_READY.md pour les deux seules cibles.
Cette attribution ne prouve pas le natif : l'auteur n'execute aucun test.

## Chemins et effets futurs explicites

- `ARTIST/verification/WorkloadProtocol.hpp` : decodeur UTF-8 scalaire partage,
  conversion UTF-16 stricte, bridge wide CRT a owners locaux, resultat/path u8,
  paire vendor/renderer et valeurs declarees avant preconditions natives.
- `ARTIST/verification/CpuUniformWorkload.cpp` et `OpenGLFloatWorkload.cpp` :
  wmain sous Windows, main sous Linux ; pas de launcher ou entree CRT forcee.
  Le tag CpuMirrorDomain du finding CPU reste le meme, aucune API GPU ajoutee.
- `ARTIST/verification/UniformWorkload.manifest` : fragment local
  urn:schemas-microsoft-com:asm.v1 / application asm.v3 /
  activeCodePage namespace http://schemas.microsoft.com/SMI/2019/WindowsSettings,
  valeur UTF-8. Aucun niveau UAC, privilege ou reglage OS global modifie.
- `ARTIST/verification/CMakeLists.txt` : /utf-8 MSVC sur les deux seules cibles ;
  /MANIFEST:EMBED et /MANIFESTINPUT:<source>/UniformWorkload.manifest ; manifeste
  declare LINK_DEPENDS. Les chemins d'options sont des arguments CMake quotes.
  Source root transmis comme literal u8 avec backslashes/quotes echappes.
  NOMINMAX/WIN32_LEAN_AND_MEAN locaux ; aucune option engine/preset/parent changee.
  Le contrat Windows est celui du CRT MSVC-compatible ; autre CRT Windows refuse
  explicitement a la configuration optionnelle, jamais traite comme Linux UTF-8.
  La documentation MSVC de /MANIFESTINPUT conserve une limite MAX_PATH de 260
  caracteres pour ce fichier : la qualification doit choisir un chemin de
  source compatible, sans ajouter de restriction ASCII ni changer l'OS.

Pas de nouveau script executable, lecteur JSON general, dependance, type de
backend ou commande de test/build ajoute. CMake configure/link ne sera execute
qu'en qualification future autorisee, pas dans cette mission.

## Oracles de lecture annonces avant implementation

Ils sont des cas de revue, **pas des tests C++ executes** :

- UTF-16 00E9 => UTF-8 C3 A9 ; D83D DE00 => F0 9F 98 80, un seul scalaire ;
  D800 seul, DC00 seul, D800 0041 => refus code30 avant chargement workload.
- UTF-8 surlong C0 AF, sequence tronquee, continuation isolee, surrogate encode
  ED A0 80 et point >10FFFF => refus ; aucun remplacement ni normalisation.
- 4096 scalaires U+1F600 (16384 octets UTF-8, 8192 unites UTF-16) admissibles
  en cible ; 4097 refuses ; C0/DEL/C1 et paire partielle/non-hardware refuses.
- Espace et e accent aigu dans un argument/path conserves : chaque argument
  reste distinct, chemin u8 devient path natif puis ofstream noreplace. Le Reader
  recoit ses paths UTF-8 narrow uniquement apres GetACP==CP_UTF8/ANSI file mode.
- Meme cible declaree/observee mais renderer llvmpipe/softpipe/SwiftShader/
  software/swrast/GDI Generic => code20/INCONCLUSIVE/UNVERIFIED avant programme,
  update/draw ; cible absente ou match seulement partiel/casse differente aussi.
- Contexte non courant, version <3.3, profil core ambigu ou fichier ACP incorrect
  ne qualifient pas hardware. Aucun nom de GPU n'acquiert une feature avancee.

Les 29 tests synthetiques Python couvrent deja les arguments/resultats/limites
et gardes de la cible, mais **ne testent pas** cette conversion UTF-16 C++, le
link du manifeste, l'ACP reel, l'acces filesystem Windows ou les calls GL.
Ne pas presenter leur prochain GREEN comme preuve de ces cas natifs.

## Environment et codes — raccordement critique

Paire optionnelle transportee sans changer les 4 parameters/resultat top-level.
La declaration est conservee dans environment meme si un bootstrap de contexte
echoue ; observations absentes sont null/UNVERIFIED, pas une cible inventee.
Le controle GL recueille vendor/renderer/version/GLSL et contexte numerique avant
tout programme/upload/draw. MATCHED_EXPECTATION exige exactitude cible, pas de
logiciel connu et vrai contexte >=3.3 core. Windows exige en plus ACP UTF-8 et
AreFileApisANSI avant le Reader classique ; controle de capability runtime,
pas devinette de version OS. Le manifeste est documente Windows1903+ mais sa
presence seule ne vaut jamais observation d'ACP effective.
L'ownership est verifie par egalite du contexte courant au GLFWwindow possede,
avant toute interrogation GL ; aucun glGetString sans contexte de la fixture.

Un code20 garde test INCONCLUSIVE, environnement UNVERIFIED, compteurs main 0
si precondition avant travail, mesure demandee ABSENT et pas de sample invente.
Le JSON reste cree exclusivement, complet si l'ecriture reussit ; stdout/stderr
restent des logs. **Collecteur : conserver ce JSON brut et le code natif avant
parser.** Son rejet par le parseur hardware strict n'efface pas la precondition
et ne devient pas automatiquement une panne pipeline ; panne ecriture/protocole
code30 et precondition code20 sont deux causes distinctes a garder visibles.

## Portee preservee et limites

Les blocs de sommets, FBO/cellules, viewports, oracles pixels et boucles chrono
ne changent pas depuis la revue shader pool-5 rapportee par le lead. Les deux
limites sont maintenant emises dans observations.workload : permutation
bijective de cellules identiques invisible ; seule derniere phase mesuree lue,
controle distinct 0/.5/0 ne prouvant pas chaque image intermediaire chronometree.
Aucun marqueur/instrumentation ajoute. Temps mural CPU reste distinct de GPU
et CPU thread ; pas de gain ou de campagne affirme.

La preuve historique shader sur SHA-256
2db9c10b7393ae100bb47ae5f01ffcb2d939c382d583ebd4446782d2ec133f80 reste bornee
a ses blocs inchanges ; elle n'accepte pas ce nouveau diff de qualification,
Unicode ou CMake. Reviewdiff independante du nouveau natif reste au lead.
Le re-audit `result_mvu7w2bo_000y_6ww1e3`, transmis ensuite par le lead, leve
statiquement le seul finding template CPU sur les objets Git de 72a382e ;
CpuMirrorDomain n'a pas change. Il ne demontre aucune compilation/execution,
n'accepte pas l'implementation Unicode/hardware de 2d212c4 et ne clot pas la
reception globale. L'absence d'objection au design Unicode n'est pas un audit
de son implementation.

Empreintes SHA-256 de cette remise source (pas des preuves d'execution) :

- OpenGLFloatWorkload.cpp :
  `1ffd3d2e8cc59a8fb4cb4b939911bb7877eb849d8df1ab0ece1a322dbbae1cdb`.
- WorkloadProtocol.hpp :
  `3b27a522951b9fdb06503ffd0ee9514c322b0be195e844b1433d96a1234dfdb3`
  apres le correctif cible nonvide de SOURCE_FIX_READY.md (ancien 2d212c4 :
  `9b2d9e821e8811de9ea8ce685185ae1dc03c37472981d65160e6bd6e64e1432b`).
- UniformWorkload.manifest :
  `ae18e973ee276ee777cf40fa0ccf4c73e23ec0280d3abd4bca39f205145ebf9a`.

Reference link lue via Context Mode :
[MSVC /MANIFESTINPUT](https://learn.microsoft.com/en-us/cpp/build/reference/manifestinput-specify-manifest-input?view=msvc-170)
confirme le merge, la necessite de /MANIFEST:EMBED et la limite du chemin.
Cela ne prouve pas l'incorporation effective dans un executable non construit.

## Remise et verification future par le lead

SOURCE_READY.md est la remise consolidee ; descriptions et selections CTest
restent sous scripts/verification/scenarios/descriptions.py (6 preparation,
7 lifetime, noms source-observes, jamais executes ni decouverts dynamiquement ici).
Les 29 tests et protocol.py sont inchanges depuis le GREEN lead. Deux tests de
caracterisation nominale espace/NBSP ont ensuite ete ajoutes sans modifier ces
assertions ; voir SOURCE_FIX_READY.md pour le seul correctif natif associe.
Commande de non-regression proposee au lead seulement :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p "test_*.py" -v
```

Attendu courant 31/31, dont 29 historiques, a observer par le lead ; executions auteur 0.
Compilation, CMake/Conan/CTest, moteur meme CPU, API GL, image, performance,
installation et cloud : **NON_EXECUTE / INCONCLUSIVE**. Aucun RED compiler
invente. La livraison globale reste conditionnee a reviewdiff et integration,
jamais a une campagne native interdite dans cette mission.
