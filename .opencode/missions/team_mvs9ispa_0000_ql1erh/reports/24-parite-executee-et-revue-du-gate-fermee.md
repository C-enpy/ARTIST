# QA Linux — parité exécutée, derniers tests du contrôleur

## Résultat vérifié

Le même correctif moteur passe **589 tests dans chacune des six configurations** : Windows, Ubuntu WSL et Ubuntu CI, en mode normal et avec AddressSanitizer (détection d'erreurs mémoire). Chaque configuration comprend 513 tests CPU et 76 tests OpenGL, sans échec, erreur ou skip. La CI est celle du correctif final, pas celle de son prédécesseur.

Le reviewer indépendant a fermé son finding sur la capacité GLEW : la correction consulte l'annonce effective de l'extension et conserve les trois fonctions obligatoires. Aucun nouveau défaut démontré dans le delta direct. Le RED et les deux GREEN Windows ont été relus par ce reviewer sans exécution de sa part.

L'arrêt interne des nouvelles admissions peut donc être levé : le défaut bloquant est corrigé, les preuves intégrées et le ré-audit reçu. Ce n'était pas une pause opérateur. Aucun changement de politique modèles/QA, nouveau GO ou redémarrage de mission.

## Dernière tranche QA

La remise publique du generator QA a été retrouvée via l'API native de sa session, sans relancer l'agent, changer ses permissions ou répéter la consultation de profils refusée. Trois compléments de tests sont retenus pour les gardes déjà présentes du contrôleur de preuves : contexte graphique manquant/incorrect, options ASan parentes hostiles et identité modifiée pendant une campagne. Aucun correctif moteur ni nouvelle infrastructure.

Le lead persiste ces propositions avant leur code puis exécute leurs tests sous Windows et Linux. Après intégration, l'audit final fresh reste requis ; l'acceptation globale n'est donc pas encore annoncée. Aucune nouvelle campagne moteur n'est nécessaire pour un ajout uniquement dans le fichier de tests Python ; les preuves valides sont réutilisées à leur portée.

## Preuves et limites

- Candidat source : `78e7b74989dd463cddf14080916e03b60405640d`. CI réelle [40760506988](https://github.com/C-enpy/ARTIST/actions/runs/40760506988), deux jobs success ; artefacts téléchargés et recoupés.
- `out/logs/mission-linux-qa/gate-final-reconciliation.json` : quatre campagnes Linux, chacune 149 hashes source/config vérifiés. Deux fichiers Windows diffèrent uniquement par CRLF ; les hashes Linux correspondent exactement aux blobs Git du candidat, et cette différence de fins de ligne est démontrée, pas ignorée.
- Windows : `gate-green-windows-{normal,asan}.xml` et découvertes correspondantes. WSL : `gate-final-linux/20261028T201504Z-8aba0173a35247e4a58c624787680584` et `20261028T201850Z-5bd88de1cef04e518c968dd062b0827a`. CI : `ci-40760506988/`.
- Noms des six populations bijectifs, après le seul cast GLM documenté. Chaque cas graphique Linux porte un contexte llvmpipe effectivement observé.
- Exécution WSL lead : clean des presets, puis `xvfb-run -a python3 -B scripts/linux-evidence.py`, avec `--address-sanitizer` pour le second passage, un job. PATH des outils réutilisé depuis l'installation ext4 existante.
- Une copie WSL vers NTFS a refusé la préservation des dates ; les octets ont été récupérés, manifestes/XML vérifiés. La seconde copie n'a pas demandé de préserver ces métadonnées. Aucun droit élargi.
- Les anciens FAIL et la limite historique du premier delta RED restent conservés. L'explication des dépendances incrémentales Windows n'est pas entièrement établie. GPU matériel Linux, Release, UBSan, performance et recherche exhaustive de fuites restent hors périmètre ; detect_leaks=0 conservé.
