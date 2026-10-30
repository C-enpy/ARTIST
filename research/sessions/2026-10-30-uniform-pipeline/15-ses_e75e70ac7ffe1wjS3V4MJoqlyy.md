# pool-4 (@correctness-reviewer teammate)

Session: ses_e75e70ac7ffe1wjS3V4MJoqlyy
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-29T23:57:49.368Z — msg_18a1931b4001oImPQdJpbOCHsl

Le HEAD observé est `57e47178d4a1d268aac0f2a88caf849df3e1ef01`, différent du candidat annoncé. Je vérifie les écarts avant de borner la revue aux sources stables. Serena est rattaché au dépôt canonique, pas au worktree : ses résultats ne prouveraient pas l’état de ce candidat.



## assistant — 2026-10-29T23:59:48.530Z — msg_18a1b032e001ZZmKxUJBV07oAG

## Revue statique — intégration bloquée

**Version :** HEAD initial `57e47178`, final `1834af6f`. Les cinq fichiers demandés sont inchangés par rapport au candidat `60f32c52`, arbre propre aux contrôles. Aucun changement `ARTIST/include` dans ce candidat.

### Défaut démontré statiquement

**P1 — `ARTIST/verification/CpuUniformWorkload.cpp:12`, lambda de `main` : type template non instancié**

- **Déclencheur :** construction de la cible `ArtistUniformCpuWorkload`.
- **Attendu :** un vecteur de contextes CPU concrets.
- **Observé :** `std::vector<artist::graphic::context::UniformContext>` utilise un nom de template comme argument de type. La déclaration réelle est `template<typename API> class UniformContext`, sans argument par défaut (`ARTIST/include/graphic/context/UniformContext.hpp:13`).
- **Impact :** programme C++ mal formé ; le producteur CPU livré ne peut pas être construit. Défaut bloquant avant qualification.
- **Vérification minimale future :** construire uniquement cette cible après choix d’une spécialisation CPU appropriée. **Compilation NON_EXECUTE** ; constat fondé sur les déclarations, sans diagnostic de compilateur.

### Question de portabilité à résoudre

**P2 potentiel — `ARTIST/verification/WorkloadProtocol.hpp:109,118`, `parse` : encodage des arguments Windows**

- **Scénario discriminant :** Windows avec page ANSI CP1252, bundle sous un chemin contenant `é`, lancement depuis Python.
- Le programme reçoit `char **argv`, mais traite systématiquement ses octets comme UTF-8. Sur ce scénario, le chemin peut être rejeté par `quote`; inversement, la construction de `filesystem::path` depuis une chaîne UTF-8 n’établit pas une conversion native Windows correcte.
- **Attendu :** création exclusive du résultat au chemin transmis ; **risque observé dans le code :** rejet ou mauvaise conversion avant production du résultat.
- **Impact :** qualification Windows dépendante d’un encodage système non déclaré.
- **Vérification minimale future :** contrôler le contrat effectif CRT/manifeste/compilation, puis lancer exclusivement le producteur CPU avec un chemin non ASCII et vérifier l’emplacement exact. **Non exécuté**, donc non présenté comme panne reproduite.

### Contrats examinés et limites

- Parsing numérique : bornes et division préalable limitent les produits à `100000000`.
- Résultat : création `noreplace`, contrôles d’écriture/fermeture, échappement JSON et validation UTF-8 présents.
- Lifetime GL : contexte englobant les ressources ; emprunt préparé consommé avant destruction du Pass ; unbinding avant destruction du programme ; restauration des pointeurs GLEW par garde.
- FBO : cellules distinctes, clear global hors boucle des cellules, readbacks hors fenêtres mesurées, oracles CPU/natif/pixels distincts structurellement.
- Aucun autre défaut démontré dans ces chemins. Cela ne constitue pas une preuve exhaustive.
- Hardware-expectation : gap connu explicitement exclu des nouvelles découvertes.
- **DEFER_TO: shader-reviewer** — confirmation géométrique des points fixes et de leurs références pixels ; pas de validation d’image dans cette revue.
- Serena pointe sur le dépôt canonique, pas ce worktree : analyse ciblée des fichiers réels, sans preuve LSP du candidat.
- Aucun test, build, rendu, benchmark ni édition effectué. Qualification Windows/Linux : **NON_EXECUTE**.

## assistant — 2026-10-30T00:11:32.356Z — msg_18a25c07f001kZbn272Zavg9N9

**Avis CPU : défaut P1 levé statiquement sur `72a382e1959fab80e6d83952719bf029352c6f7e`.**

- `CpuUniformWorkload.cpp:10` définit désormais `CpuMirror = UniformContext<CpuMirrorDomain>` ; le vecteur ligne 21 reçoit donc un type concret.
- La définition relue par objet Git confirme que le paramètre `API` n’est pas utilisé dans le stockage ni dans `setValue/getValue`. Tag complet, local au fichier ; boucles et oracles inchangés.
- Aucun changement moteur/API nécessaire. **Compilation et exécution : NON_EXECUTE** ; seul le défaut identifié est clos.

HEAD observé : `82e6cc849de2fee9555c745863d9e0bf9e226e9b`. Revue ancrée aux objets Git, sans lecture des sources en cours de mutation.

**Unicode :** proposition documentaire lue ; aucune objection concrète au design ratifié. La combinaison entrée wide, conversion stricte, chemins natifs, manifeste local et garde préalable au Reader traite les frontières identifiées. Cela ne valide pas encore son implémentation ni le comportement CRT/STL effectif, à réauditer après SOURCE_READY.

Aucun test, build, rendu ou changement effectué. Limites précédentes conservées, sans nouvelle enquête géométrique.

## assistant — 2026-10-30T00:33:42.746Z — msg_18a3a0d56001VQxFcvSarWs56j

## Ré-audit SOURCE_ONLY — un écart de contrat

**Candidat vérifié :** `fa828094e67f9ec17eb7b76e38537627d04b0810`, HEAD inchangé entre les contrôles, arbre propre. Diff examiné depuis `82e6cc84`. Aucun changement `ARTIST/include`.

### P2 — bloquant pour la conformité du protocole cible

**`ARTIST/verification/WorkloadProtocol.hpp:190`, `parse` — cible composée d’espaces rejetée comme erreur protocole**

- **Scénario discriminant :** paire hardware complète, avec `expected_gl_vendor=" "` et un renderer non vide.
- **Attendu :** cette chaîne non vide, sans contrôle et sous 4096 scalaires est admise par le contrat et par `_target_text` (`scripts/verification/scenarios/protocol.py:61–69`). Une discordance avec le vendor observé doit produire **code20 / UNVERIFIED**, avec observations conservées.
- **Observé statiquement :** le filtre générique `find_first_not_of(" \t\r\n\f\v")` rejette la valeur avant `validateTarget`, avant création du résultat et avant observation GL. Retour **code30**, sans JSON.
- **Impact :** divergence Python/natif ; une cible admissible mais discordante devient une panne de protocole au lieu d’une précondition indisponible.
- **Vérification minimale future :** distinguer ce cas d’une chaîne vide et d’une cible contenant un contrôle ; vérifier ensuite le chemin de discordance code20 et son résultat brut. **Non exécutée.**

### Contrôles statiques sans autre défaut démontré

- UTF-8 : bornes, continuations, surlongs, surrogates et valeurs au-delà de `U+10FFFF` contrôlés. `U+FFFF` reste admis : c’est un scalaire Unicode, malgré son statut de non-caractère.
- UTF-16 : paires validées ; surrogates isolés refusés. La limite cible compte les **scalaires**, pas les octets/unités UTF-16.
- Bridge `wmain` : construction de tous les owners avant emprunt des buffers ; durée de vie couvrant l’appel partagé.
- Chemins : conversion explicite par `u8string`, résultat ouvert exclusivement ; racine shader typée UTF-8.
- CMake : options, manifeste et `LINK_DEPENDS` limités aux deux workloads.
- GL : contrôle du contexte possédé, cible exacte et garde ACP/file API avant Reader/programme/uploads/draws ; environnement déclaré initialisé avant bootstrap, observations conservées avant les refus ordinaires de qualification.
- Ordre de destruction contexte/Pass/ressources inchangé ; défaut CPU précédemment levé non réintroduit.

**Limites :** aucune compilation, aucun test, rendu ou benchmark. Incorporation effective du manifeste, comportement CRT/STL et exécution Windows/Linux restent **NON_EXECUTE**. Pas de nouvelle revue géométrique ; l’absence d’autre finding n’est pas une preuve exhaustive.

## assistant — 2026-10-30T00:39:24.319Z — msg_18a3f439b0010UbsJ9ppMA526X

**Finding P2 levé statiquement sur `1eaa1875cd3757fc94ab84f4c4992a5124ce6781`.** HEAD conforme, arbre propre.

Dans `WorkloadProtocol.hpp`, `parse` distingue désormais correctement :
- les deux options cible : chaîne **non vide**, espaces conservés ;
- les autres options : filtre historique inchangé.

Les validations ultérieures paire/profil, UTF-8, contrôles et limite de 4096 scalaires restent inchangées. Une cible `" "` peut donc atteindre la comparaison native et son refus code20, au lieu du rejet prématuré code30.

Aucune régression supplémentaire démontrée dans ce petit diff. Aucun finding bloquant restant dans le périmètre revu.

**Avis : favorable à la réception SOURCE_ONLY.** Compilation, tests natifs, incorporation du manifeste et fonctionnement Windows/Linux restent **NON_EXECUTE**. Aucun test ni modification effectué ; le résultat des 31 tests annoncé par le lead n’est pas une preuve native.