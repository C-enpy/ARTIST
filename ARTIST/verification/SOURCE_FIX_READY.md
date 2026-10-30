# SOURCE_FIX_READY — cible non vide, pas non blanche

## Finding et autorisation

Re-audit independant annonce par le lead : `result_mvu8p34c_0000_6de7kv`.
Cause recoupee dans WorkloadProtocol.hpp : le filtre generique
find_first_not_of(" \\t\\r\\n\\f\\v") rejette une valeur cible " " avant
validateTarget et avant creation JSON. Le contrat ratifie et Python acceptent
pourtant toute chaine **non vide** sans controle dans la borne de 4096 scalaires.
Le lead exige de conserver ce contrat exact, sans strip/normalisation.
Ce finding est statique, pas un echec natif/compilation reproduit.

## Correction source bornee

Seules les options --expected-gl-vendor/--expected-gl-renderer, reconnues en GL,
emploient maintenant le predicat !value.empty(). Toutes les autres options
conservent leur filtre historique. Le code de reconnaissance, les doublons,
la paire complete, le profil hardware, la validation UTF-8/controles/4096 et
le chemin d'erreur restent actifs ; aucune branche d'update/draw n'est touchee.

Chemins relus, sans execution :

- cible ASCII espace : nonempty vrai ; validateTarget lit U+0020, non controle,
  1 scalaire ; valeur conservee exactement dans Request et environment ;
- cible NBSP : nextUtf8 decode C2 A0 en U+00A0, hors C0/DEL/C1, 1 scalaire ;
  valeur conservee, sans normalisation vers U+0020 ;
- cible vide : refus code30 avant fichier ; tab/NUL/autre controle : refus
  validateTarget code30 avant programme/upload/draw ; 4097 scalaires aussi ;
- cible valide differente des GL strings observees : targetMatches faux,
  environment garde declaration/observation UNVERIFIED, puis code20 natif,
  INCONCLUSIVE et mesure demandee ABSENT. Le JSON brut complet est conserve ;
  le collecteur ne doit pas transformer un rejet du parser strict en panne
  pipeline si la cause native est cette precondition code20.

Pas de changement du natif GL, main/wmain, Unicode, CMake/manifeste, geometrie,
chrono, CPUtag ou des quatre parameters. Pas de backend ou harness ajoute.
WorkloadProtocol.hpp SHA-256 apres ce correctif :
`3b27a522951b9fdb06503ffd0ee9514c322b0be195e844b1433d96a1234dfdb3`.

## Caracterisations Python ajoutees avant correction

`scripts/tests/verification_scenarios/test_target_whitespace.py` ajoute deux
tests nominaux sur paires espace/NBSP : argv exact et resultat fake correctement
attribue. Les observations de contexte/GPU restent purement synthetiques.
Le protocole Python fonctionnel est **inchange** et accepte deja ces valeurs :
caracterisation, aucun RED attendu ou fabrique. Aucun modele core copie ; le
filtre du Request core et son propre RED restent au proprietaire core.
Les 29 assertions historiques restent strictement inchangees.

Commandes proposees au **lead uniquement**, pas executees par l'auteur :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p "test_target_whitespace.py" -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p "test_*.py" -v
```

Attendu : 2 nouveaux tests nominaux verts, puis 31/31 dont 29 historiques
inchanges. Ces nombres sont des definitions/attendus, pas une execution de
l'auteur ni la preuve de cette branche C++. Exemples natifs ci-dessus :
oracles de revue uniquement, aucune compilation ou execution autorisee.

## Limites et reception

Executions auteur 0. Compilation, CMake/Conan/CTest, moteur CPU/GL, image,
performance, installation et cloud : NON_EXECUTE / INCONCLUSIVE.
La revue independante ne signalait pas d'autre defaut concret Unicode/owners/
CMake/GL, sans les valider nativement. Ce correctif exige re-audit du diff et
non-regression lead puis integration ; aucune reception globale annoncee.
