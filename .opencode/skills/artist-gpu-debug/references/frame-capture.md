# Frame/capture : trouver la première divergence, pas réécrire le rendu

Méthode documentaire, aucune capture ni mesure réalisée par le chargement de ce
fichier. `rendering-verifier` travaille en lecture seule ; l'auteur du correctif
est identifié via le lead après le diagnostic. Source, tests et goldens intacts.

## Choisir la preuve et le canal utile

1. Identifier candidat HEAD/delta/hashes, binaire/configuration, shaders/defines,
   assets, API/profil/capacités, GPU/driver, seed/caméra/résolution/frame/temps.
   Définir attendu, métrique/tolérance et hypothèse avant inspection comparative.
2. Pour une `.rdc` existante : provenance/hash, outil/version de capture, API et
   GPU d'origine, compatibilité du replay et environnement actuel. Le replay peut
   modifier timing/ordonnancement ; une image rejouée n'est pas une mesure native.
3. Partir de la sortie fausse, identifier **event/draw**, pass, shader/variante,
   bindings et attachment. Remonter au premier pass incorrect dont les entrées
   sont encore correctes, puis réduire sans changer plusieurs facteurs à la fois.
4. Choisir ce qui discrimine l'hypothèse : couleur RGB/alpha, profondeur/stencil,
   mip/couche/sample, valeur brute ou HDR avec plage d'affichage fixée. Un canal
   noir dans une vignette peut contenir de vraies valeurs hors plage. Vérifier
   conversion linéaire/sRGB, orientation, exposure, resolve et tonemapping.
5. Examiner les entrées : mesh/indices/attributs et sortie vertex, matrices,
   textures/samplers/uniforms, viewport/scissor, cull/depth/stencil/blend ; comparer
   la sortie du draw avec celle du pass suivant. Lire logs/API debug disponibles.
6. Conserver événement exact, image/canal/plage, attendu/observé et première
   divergence. Ne pas inférer un défaut de visibilité d'une simple image noire.
   Une attente globale est une expérience distincte de l'auteur autorisé, pas un
   patch automatique du verifier ni une mesure de performance.

**Nominal** : pass isolé, entrées identifiées, attachment et canal reproduisent
l'attendu sur replay compatible puis confirmation sur cible si le critère l'exige.
**Négatif** : mauvais attachment/canal, capture périmée, frame différente ou replay
incompatible → corriger l'expérience ou INCONCLUSIVE, pas un faux PASS visuel.
Un défaut effectivement reproduit reste FAIL même si un outil manque ailleurs.

## Outils conditionnels, sans installation

Vérifier existence, version, API/OS/GPU supportés et permissions **avant** l'usage.
Le harness ne fournit **aucun MCP** RenderDoc/Nsight/PIX. RenderDoc n'est pas une
capture universelle Metal ; ce cas demande une voie Apple autorisée et disponible.

- Voie utilisable par défaut si fournie : capture `.rdc` existante dans RenderDoc
  installé/autorisé ou **fallback manuel** demandé à l'opérateur (event/draw,
  états, images intermédiaires et logs). Ne pas prétendre inspecter un fichier
  binaire si aucun replay/lecteur n'est accessible. Analyse offline des exports
  seulement : noter ce qu'ils ne permettent pas de conclure.
- **rdc-cli** est un **candidat documentaire**, **non installé**/non activé par ce
  lot. L'audit T02 ne l'a pas résolu dans le PATH, pas une preuve d'absence partout.
  Pas d'appel supposé à `rdc doctor` ni de commandes devinées. Avant adoption
  séparément autorisée : vérifier version, module Python RenderDoc compatible,
  `renderdoc.pyd`/chemins éventuels, support replay et aide réelle. L'ouverture
  peut démarrer un daemon : bail, capture active, chemin d'artefacts et fermeture
  explicites ; ne pas laisser un daemon ou une capture vivante après usage.
- [CLI-Anything](https://github.com/HKUDS/CLI-Anything) : alternative documentaire,
  pas deuxième adaptateur installé « au cas où ». Motiver tout avantage et vérifier
  dépendances/effets de bord séparément. **aucune installation** ni exécution de
  code tiers au chargement de ce skill, aucun téléchargement de SDK/driver.
- Nsight/PIX/autre : découverte effective et support exact seulement. Une CLI sur
  le PATH ne prouve ni capture réussie, ni counters disponibles, ni benchmark.

Respecter le bail du lead pour capture/replay GPU. Sans permission d'artefacts,
le verifier restitue du texte au lead/runner ; aucun contournement shell/MCP.
Une capture peut contenir assets, chemins et shaders sensibles : pas d'upload
externe implicite. Conserver originaux/hash ; ne pas réenregistrer le candidat.

## Rapport et limites des preuves

Symptôme → hypothèse → expérience → observé → conclusion ; candidat et hash,
commandes/outils/versions, frame/event/pass, paramètres de vue, chemins d'artefacts,
messages driver pertinents et prochaine expérience discriminante.

- `executed` : commande réellement exécutée et résultat, préconditions, nombres
  découverts/exécutés/échoués/skipped, cible et temps d'observation.
- `static-presence` : procédure, texte, fichier ou outil présent seulement.
- `inference` : mécanisme plausible non reproduit, à confirmer explicitement.

Validation CPU/API, image et performance ont des verdicts **séparés**. Un mock,
zéro test, test GPU skipped ou rendu logiciel n'établit pas PASS sur GPU cible.
Une capture visuelle n'est pas un benchmark : performances CPU submit/GPU/waits
requièrent protocole et mesures répétés hors perturbation de capture ; transmettre
au lead `DEFER_TO: performance-reviewer`. Aucun gain chiffré sans mesure.

## Sources et dispositions

Rédaction originale à partir des obligations T03/QA et de la méthode ARTIST, pas
copie d'un skill tiers. Sources officielles de consultation :
[documentation RenderDoc](https://renderdoc.org/docs/) et
[projet RenderDoc](https://github.com/baldurk/renderdoc). Vérifier version, support,
licence et notice des fichiers précis avant toute adaptation.

[renderdoc-skill](https://github.com/rudybear/renderdoc-skill) : candidat référencé,
pas importé. T02 rapporte MIT au niveau dépôt et le **SHA d’arbre**
`de33576fa0a4ee739b4b0434d10d710880d604c6`, **pas un commit** épinglé. CLI-Anything :
Apache-2.0 signalée au dépôt, arbre `34f519533bc175d2fe287ab8316b0dd99bb9cc43`.
Ces observations du 2026-10-16 ne dispensent pas de résoudre un commit exact et
de conserver la notice avant copie/adaptation. Licence inconnue = lien seulement.
L'absence d'outil ne bloque pas ce lot documentaire ; elle limite la preuve réelle.
