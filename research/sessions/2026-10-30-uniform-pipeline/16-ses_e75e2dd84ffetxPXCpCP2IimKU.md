# pool-5 (@shader-reviewer teammate)

Session: ses_e75e2dd84ffetxPXCpCP2IimKU
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T00:03:23.754Z — msg_18a1e4be7001zEEt6QDf7vb2NC

## Conclusion

**Aucun défaut bloquant démontré dans la géométrie ou les quatre pixels attendus.** Les oracles sont mathématiquement cohérents et indépendants d’une égalité A/B. Deux limites de détection doivent rester explicites.

**Compilation/rendu : NON_EXECUTE. Conclusions visuelles : INCONCLUSIVE.** Aucun moteur, build, rendu ou benchmark lancé ; aucune modification.

### Candidat observé

Worktree `ensemble-2dc31e0a-cwsmsw-uniform-scenarios`, HEAD lu : `cbf39ce183c61cd62c2b3a8a48ab220e5b4214af`.

SHA-256 :
- `ARTIST/verification/OpenGLFloatWorkload.cpp` : `2db9c10b7393ae100bb47ae5f01ffcb2d939c382d583ebd4446782d2ec133f80`
- `f2-single-values.vert` : `9edc5dbaad84917ccc2fcc0fd61113b30c3e8f8413dc94c9f51fad97b647c86e`
- `minimal.frag` : `d7f1a8dc44c051cf0e209ecd0160c7f88c0ab590fc2a48f74696ec4421301bd4`

Bloc viewport/oracle relu en fin de revue, inchangé. Ces observations ne constituent pas un gel transactionnel contre les modifications concurrentes. Axe hardware/protocole exclu comme demandé.

## Vérification mathématique

Localisation : `OpenGLFloatWorkload.cpp:255–275,294–340,582–588` ; vertex shader `:10–11`.

Avec matrice identité, vecteurs et array nuls :
`gl_Position = vec4(position + vec3(s), 1)`.

Coordonnées locales fenêtre, origine bas-gauche :
- `s=0` : sommets `(4,4), (28,4), (16,28)`.
- `s=.5` : sommets avant clipping `(12,12), (36,12), (24,36)`.

Les centres échantillonnés sont `(x+.5,y+.5)` :
- `(8,8)` : blanc puis noir ;
- `(24,24)` : noir puis blanc ;
- `(18,16)` : blanc aux deux phases ;
- `(2,2)` : noir aux deux phases.

La marge minimale des points blancs aux arêtes est d’environ **2,01 pixels**, pour `(8.5,8.5)` à phase zéro. Aucun point ne dépend d’une règle de couverture sur une arête. Le clipping de la phase `.5` préserve les points retenus et empêche le débordement dans les cellules voisines.

Le déplacement affecte également `z` (`0 → .5`), mais reste dans le volume clip OpenGL ; depth test désactivé. Aucun espace monde/vue, UV, normale, éclairage ou interpolation de couleur n’intervient.

**ABI/états observés :** attribut location 0, trois `GL_FLOAT` non normalisés, stride `3*sizeof(GLfloat)` ; uniforms GLSL compatibles avec les initialisations présentes. FBO RGBA8 linéaire monosample, fragment blanc constant opaque, clear noir opaque ; blend/dither/sRGB désactivés. La comparaison exacte aux extrêmes 0/255 est justifiée ici, sans élargir une tolérance driver.

## Pouvoir discriminant établi statiquement

- **Draw absent dans une cellule :** `(18,16)` reste noir après clear, alors que blanc attendu.
- **Valeur figée à zéro :** phase `.5` échoue aux deux points mobiles.
- **Valeur figée à `.5` :** phase zéro échoue ; le retour final à zéro vérifie aussi la réversibilité.
- **Viewport figé ou cellule omise avec duplication ailleurs :** cellule non dessinée détectée.
- **Update omis malgré valeur répétée identique :** les pixels seuls ne suffisent pas, mais le contrôle distinct compte les uploads et vérifie le scalaire natif par cellule (`:411–446`).
- **Clear :** effectué une fois par phase, hors boucle cellule ; scissor désactivé. Les cellules précédentes ne sont pas effacées.

Les deux routes partagent shaders, initialisations, géométrie, phases et oracle : qualité analytique comparable.

## Limites non bloquantes / suggestions

### 1. Une permutation bijective des cellules est indétectable

**Localisation :** `:294–298,313–326,393–398`.

Toutes les cellules reçoivent la même valeur et la même géométrie. Remplacer conceptuellement `viewport(cell)` par `viewport((cell+1)%batch)` visite encore chaque cellule une fois et produit exactement la même image.

**Impact :** ne pas présenter cet oracle comme détectant toute « mauvaise cellule ». Il détecte omissions/duplications, pas une permutation complète.

**Test discriminant proposé, non exécuté :** mutant de permutation pour batch 16. Si l’identité logique de chaque cellule devient un critère, prévoir un marqueur indépendant par cellule ; ce serait une extension du contrat actuel, pas une correction nécessaire aux pixels présents.

### 2. Les fenêtres chronométrées ne vérifient que leur dernière phase

**Localisation :** `:490–500,513–518`.

Le contrôle distinct vérifie bien les trois phases. En revanche, une anomalie limitée aux phases intermédiaires d’une fenêtre mesurée peut être invisible si sa dernière phase est correcte.

**Impact :** les pixels finaux ne prouvent pas chaque draw/update chronométré ni l’absence de scintillement. Cette limite est compatible avec la séparation contrôle/mesure prévue ; aucune revendication plus forte ne doit en être déduite.

**Test discriminant proposé, non exécuté :** omettre uniquement une phase intermédiaire dans le chemin mesuré tout en conservant la dernière.

## Limites de portée

Référence historique localisée : `ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformRenderTests.cpp:132–139`, mêmes points fixes. Présence statique seulement.

Identité et zéros ne qualifient pas transposition matricielle, packing général, autres types d’uniforms, profondeur ou normales. Ces sujets, ainsi que lifetime général et performance, restent hors de cette revue. Livraison **pour qualification ultérieure**, sans PASS graphique.