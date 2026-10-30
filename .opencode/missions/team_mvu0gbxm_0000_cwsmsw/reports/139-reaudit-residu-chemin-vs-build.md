# 139 — Ré-audit : résidu du chemin explicite en construction

Le reviewer confirme les corrections du cache typé et de la copie avec Keys.
La comparaison sélection/export Visual Studio est également corrigée, mais
un garde antérieur en phase build conserve le même défaut de séparateur final.

Prepare peut recevoir `C:\VS\` et publier le témoin normalisé `C:\VS`. Build
recevant le même argument explicite le compare encore avant normalisation et
le refuse. Le parcours habituel de l'adaptateur sans cet argument n'est pas
concerné, mais l'équivalence des installations explicites reste sous contrat.
L'intégration demeure donc bloquée ; F1 n'est pas entièrement fermé.

Le coordinateur demande au propriétaire une régression isolée avant correction :
contrôles prepare avec suffixe/build sans suffixe et refus d'une installation
réellement différente, puis cas build avec suffixe contre témoin normalisé.
Les données doivent conserver le suffixe dans l'argument, pas le supprimer dans
la fixture. La véritable couture d'un même témoin reste une vérification ultérieure,
distincte de ce test du garde.

La production et ses pins restent stables. Les résultats verts du rapport138
gardent leur portée ; aucun nouveau test ou défaut dynamique n'est revendiqué
pour ce résidu relevé en lecture.

## Références et limites

- Ré-audit `result_mvuwbbib_007v_9g4lwc`, pool-36, lecture seule, sans test,
  processus, édition ou délégation.
- Candidat `9680f2d9c47e69ddd538e9c01044ac8423d4a304` ; source ead7f837.
- `Read-VerificationMsvcEnvironmentLeaf:976` : garde build avant normalisation
  lignes997–1001. Le test F1 existant couvre le suffixe exporté, pas cette entrée.

Encodage non ASCII, qualification native, couture intégrée et gap Dispose/writers
restent les limites annoncées. Aucun moteur, outil natif, GPU ou cloud exécuté.
