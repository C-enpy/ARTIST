# Revue source — correctif CPU et proposition Unicode Windows

> Historique de proposition/revue. Le lead a ensuite ratifie Unicode et observe
> GREEN hardware 29/29. Voir SOURCE_READY.md et NATIVE_UNICODE_AND_TARGET_READY.md
> pour l'etat SOURCE_ONLY courant ; les mentions « non applique » ci-dessous
> decrivent la remise de proposition, pas le dernier candidat natif.

## Finding bloquant CPU et correctif borne

Finding independant pool-4, resultat annonce par le lead :
`result_mvu7hkm1_000m_zkrp5n`. Le lead et l'auteur ont recoupe la declaration
`ARTIST/include/graphic/context/UniformContext.hpp:13` :
`template<typename API>`, sans argument par defaut. La source precedente
declarait vector<UniformContext>, donc employait un template sans argument.
Ce constat est une analyse de source, **pas une panne de compilation reproduite**.

`CpuUniformWorkload.cpp` emploie maintenant un alias
UniformContext<CpuMirrorDomain>, avec un tag prive au fichier. Le parametre API
n'est utilise dans aucune operation du stockage CPU existant : setValue et
getValue gerent std::any, sans appel GL. Aucun backend fictif, type OpenGL,
composant mock ou changement ARTIST/include n'est introduit. Les boucles,
oracles, populations et resultats CPU restent identiques.

Ce correctif est SOURCE_READY pour re-audit independant ; compilation et
execution CPU restent NON_EXECUTE. Il ne change pas les 29 tests Python, le
protocole Python hardware ou le natif GL. Il ne transforme pas le GREEN Python
en preuve de compilation C++.

## Risque distinct Unicode Windows — observations statiques

Dans WorkloadProtocol.hpp, parse recoit actuellement char** en supposant UTF-8,
puis construit resultPath depuis std::string narrow. Sur Windows, ces deux
frontieres ne garantissent pas la conservation Unicode pour une CRT utilisant
une page de codes historique (exemple CP1252 avec e accent aigu). C'est un
**risque deduit**, pas une panne reproduite ni un encodage effectif observe.

Lectures et recherches exactes du worktree :

- CMakeLists.txt fixe C++23 ; aucun reglage Unicode/CRT de cette frontiere.
- ARTIST/verification/CMakeLists.txt utilise add_executable console ordinaire,
  sans WIN32, /ENTRY, manifeste ou /utf-8.
- Aucun reglage cible de activeCodePage, UTF-8, wmain, UNICODE, /ENTRY ou
  SUBSYSTEM trouve dans cmake/, CMakePresets.json, build-windows.ps1 ou la CI
  cmake-single-platform pour imposer ce contrat.
- Aucun fichier .manifest/.rc/.props trouve par le glob projet. Cela ne prouve
  pas les flags d'un futur build, l'environnement CL ou un manifeste genere.
- Le Reader de production existant appelle ifstream.open sur
  ShaderContext::getShaderPath(), qui retourne std::string. **Corriger seulement
  argv et resultPath laisserait donc aussi la lecture des shaders depuis une
  racine source Unicode a traiter.** Aucun fichier moteur modifie ici.

## Proposition complete a ratifier avant implementation

1. Sous Windows/MSVC, utiliser wmain(argc, wchar_t**) avec le demarrage CRT
   console normal : pas de /ENTRY force, _tmain, changement de locale systeme
   ou reinterpret_cast des octets UTF-16 en char. Convertir explicitement les
   scalaires UTF-16 en UTF-8 avec validation stricte des surrogates, dans un
   support local standard library. Conserver l'ownership des chaines jusqu'au
   retour du parseur et du workload. Linux garde main et verifie UTF-8.
2. Toutes les chaines du protocole en memoire/JSON sont UTF-8. La limite cible
   4096 reste un nombre de scalaires Unicode, pas d'octets UTF-8 ni d'unites
   UTF-16 (une paire de surrogates compte pour un caractere). Pas de restriction
   ASCII, de translitteration, de remplacement silencieux ou normalisation NFC
   qui changerait la comparaison exacte attendue/declarée/observee.
3. Construire resultPath via un std::u8string et filesystem::path, pas une
   conversion narrow dependante d'ACP. L'ofstream conserve ce path natif et
   noreplace. Construire aussi la racine source depuis un literal UTF-8 typé,
   puis transmettre explicitement les bytes u8string pour les paths shaders.
4. Pour conserver le Reader Classique **sans modifier ARTIST/include**, proposer
   un manifeste UTF-8 **local aux deux workloads** (activeCodePage=UTF-8), avec
   /utf-8 cible MSVC pour le source/litteral de la racine. /utf-8 seul ne regle
   ni argv CRT ni les paths narrow du Reader. Aucun changement global du projet.
   Le manifeste serait incorpore explicitement au link des seules cibles, sans
   modifier leur niveau UAC ou imposer un launcher.
5. Cette derniere partie exige Windows >=1903 pour le comportement documente
   activeCodePage. Verifier explicitement dans le workload GL l'ACP UTF-8 et le
   mode de fichiers ANSI avant de lire des paths narrow du Reader ; sinon
   precondition indisponible, code20/INCONCLUSIVE, jamais fallback ou limitation
   aux noms ASCII. GetACP/AreFileApisANSI seraient les seuls controles Win32
   proposes (SDK/kernel32 deja dans la toolchain, pas nouvelle dependance).
   L'entree wide et resultPath natif ne dependent pas de cette ACP.

Le choix manifeste et ses preconditions de plateforme est ici une **proposition**,
pas un patch applique ou un support Windows deja demontre. Si le lead refuse
cette precondition, une alternative doit eviter la frontiere narrow du Reader
(ownership moteur interdit ici) plutot que pretendre que /utf-8 ou wmain seul
corrige tout. Pas de SDK, package, driver, backend ou systeme de fichiers ajoute.

## Sources documentaires verifiees sans execution native

Microsoft Learn, pages fetch/index via Context Mode, extraits consultes :

- [wmain et argv wide](https://learn.microsoft.com/en-us/cpp/cpp/main-function-command-line-args?view=msvc-170)
  : point d'entree Microsoft wchar_t ; _tmain depend de _UNICODE.
- [/utf-8](https://learn.microsoft.com/en-us/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8?view=msvc-170)
  : fixe les character sets source/execution, pas un contrat de transport CRT.
- [Code page UTF-8](https://learn.microsoft.com/en-us/windows/apps/design/globalizing/use-utf8-code-page)
  : activeCodePage supporte a partir de Windows 1903 ; autrement ACP peut
  rester la page historique, meme si le manifeste est present.
- [fopen/_wfopen](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-wfopen?view=msvc-170)
  : filename narrow interprete CP_ACP par defaut, wide explicite distinct ;
  ne pas confondre encodage du nom avec celui du contenu lu.

Ces references et l'analyse du Reader motivent la proposition. Elles ne sont
pas une observation de la CRT/STL/link exacts de la future campagne.

## Preuves et prochaines barrieres

Executions auteur : 0 ; aucune compilation, CMake/Conan/CTest, suite moteur meme
CPU, GL, rendu, benchmark, installation ou cloud. Aucun test/assertion Python
change. Le candidat GREEN Python precedent est remis separement en cbf39ce ;
aucun resultat de cette nouvelle source n'est attribue a ses 29 tests.

CPU : cause statique corrigee, re-audit et compilation future requis.
Unicode : risque identifie, proposition complete, implementation NON_EXECUTE et
encore non appliquee. Pixels/equations : avis independant recu ci-dessous ;
ni cette analyse ni la revue C++ ne prouvent une image correcte.

### Avis shader recu ensuite — portee distincte, sans lever les autres gates

Le lead transmet l'avis independant pool-5 `result_mvu7m96w_000s_r34ufc` :
aucun defaut bloquant demontre sur l'axe geometrie/pixels du candidat cbf39ce.
Le hash SHA-256 natif relu est toujours
`2db9c10b7393ae100bb47ae5f01ffcb2d939c382d583ebd4446782d2ec133f80` dans ce
worktree. Aucune nouvelle enquete ou execution graphique n'est requise pour
reutiliser cet examen des blocs inchanges.

Conserver deux limites non bloquantes : permutation bijective de cellules
identiques invisible ; readback du chemin mesure limite a sa derniere phase,
le controle separe 0/.5/0 ne prouvant pas chaque image intermediaire mesuree.
SOURCE_READY.md les detaille et reserve leurs formulations d'observations au
prochain patch natif autorise, sans ajouter d'instrumentation ou changer le
source pendant la gate GREEN du parseur hardware.

Cet avis ne leve ni la reception/re-audit du correctif CPU, ni le risque Unicode,
ni l'interdiction de compilation/execution. API/image/performance restent
NON_EXECUTE ; aucun PASS graphique ou gain n'en est deduit.
