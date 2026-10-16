# Ledger et traduction des hazards — référence ciblée

Rédaction originale, 2026-10-16. Modèle de raisonnement, pas nouveau gestionnaire
de ressources ARTIST ni test exécuté. Ne pas généraliser les mécanismes d'une API.

## Ledger minimal, à remplir avant de modifier

- Identité logique, génération ; owner et borrow, moment où les emprunts cessent
  d'être valides. La génération ne constitue pas une attente GPU.
- Identité allocation/heap, plage d'octets, taille, alignement, résidence/budget,
  sous-ressource (mip, couche, aspect), format, stride/offsets et usage autorisé.
- Vues, binding et descripteur, durée de vie du storage et des données référencées.
  Les slots de descripteurs en vol ne sont pas immédiatement recyclables.
- Producteur puis chaque consommateur : frame, pass, queue/contexte/thread,
  lecture/écriture, domaine host/copy/compute/graphics, stages, accès, état/layout.
- Dépendances d'exécution et de visibilité ; complétion `lastUse[q]` par queue,
  token observé `completed[q]`, état retiré et propriétaire de la destruction.
- Échecs : création partielle, upload rejeté, remplacement/resize, device/context
  loss ; ne pas publier d'objet partiellement prêt ni appeler un device détruit.

Nominal conceptuel : destination B, génération 4, copy C9 puis graphics G17,
avec l'arête GPU C9 → G17. Si compute K4 lit aussi B, le retrait exige
`completed[q] >= lastUse[q]` pour **toutes les queues** utilisatrices. Comparer
G17 à C9 numériquement est dépourvu de sens : timelines indépendantes. Une queue
finale peut agréger les dépendances seulement si cette jointure est prouvée.
Staging S, sans autre utilisateur, peut être recyclé après C9 ; pas B.

Négatif : `completed = {C:9,G:16,K:4}`, `lastUse = {C:9,G:17,K:4}`.
Oracle de retrait : B reste détenu, même avec génération 5 publiée. Avec G17
achevé et tous les borrows CPU expirés, B peut être détruit dans le bon contexte.
Tester aussi dernière utilisation inconnue, token périmé après recréation device,
et fenêtre où un nouveau submit arriverait après la décision de retrait : fermer
les admissions/leases avant le snapshot. Ni index de frame ni délai arbitraire.

## RAW, WAR, WAW : trois besoins à séparer

Pour chaque hazard, justifier ordre d'exécution, visibilité mémoire et attente CPU
indépendamment : aucune de ces trois preuves ne remplace automatiquement les autres.

- **RAW** : écriture → lecture. Assurer ordre et visibilité des écritures de la
  plage requise pour le domaine consommateur. Un accès indirect/vertex n'est pas
  interchangeable avec lecture texture. Pas de flush de toutes les ressources.
- **WAR** : lecture → écriture destructive. Empêcher l'écrasement avant la fin
  des lectures. Un WAR pur demande une dépendance d'exécution, pas la visibilité
  d'une écriture antérieure inexistante ; transitions/aliasing peuvent ajouter des
  obligations propres à l'API, à justifier séparément.
- **WAW** : écritures chevauchantes dont l'ordre importe. Ordonner les écritures
  et respecter les dépendances mémoire/cache exigées par l'API. Déclarer si la
  deuxième écriture remplace toute la plage ; une écriture partielle ne permet
  pas de jeter les données restantes. Rejeter les writers non ordonnés.
- **Attente CPU** : nécessaire seulement à la réutilisation/lecture host ou à un
  contrat explicite. Une arête GPU n'impose pas automatiquement un blocage CPU.
  Attendre la complétion ne remplace pas flush/invalidate host non cohérent requis.

## Traduction conceptuelle, aucun résultat GPU revendiqué

### Vulkan

`vkCmdPipelineBarrier2` si synchronization2 effectivement supportée et activée :
scope stages/access source/destination, plage et layouts d'image justifiés.
Sinon chemin compatible avec l'API disponible, pas appel de symbole supposé.
Inter-queues : sémaphore (éventuellement timeline) + portée appropriée ; transfert
de propriété de famille pour ressource exclusive quand requis. Une fence permet
au CPU d'observer la complétion, pas une transition de layout. Host non cohérent :
flush avant consommation GPU, invalidate après complétion avant lecture CPU ;
respecter nonCoherentAtomSize et alignements device, sans les coder arbitrairement.

### D3D12

Transition de state, UAV barrier et aliasing barrier ne sont pas synonymes.
Pour copy → graphics, identifier l'état permis par chaque type de queue puis
`Signal`/`Wait` de fence sur queues et transitions applicables. Ne pas remplacer
une dépendance inter-queues par une barrière locale. Enhanced barriers seulement
si supportées. Allocator, ring upload, heap/descripteurs et ressource gardent leurs
propres derniers utilisateurs ; une fence CPU pour un allocator ne valide pas
le state du buffer. Résidence disponible ≠ durée de vie gérée automatiquement.

### Metal

Distinguer storage mode, hazard tracking et ressources/heap aliasés ou indirects.
Tracking automatique ne couvre pas n'importe quel accès indirect ou non tracké.
`MTLFence` pour les dépendances d'encodeurs là où l'API le permet ; événements
(partagés si requis) pour coordination appropriée, handlers/attente de complétion
pour observation host. Vérifier famille/OS/SDK et modèle de commandes avant de
choisir la primitive, notamment Metal 4. Ressource retained ou autorelease pool
vivant ≠ achèvement GPU ; storage managed peut exiger synchronisation explicite
pour lecture CPU. Pas d'exécution Metal sur Windows déduite d'un MSL généré.

### OpenGL

Contexte courant/thread, partage et objets non partageables sont des préconditions.
Le flux GL usuel ordonne ses commandes ; pas d'état de ressource explicite global.
Une écriture shader incohérente suivie d'une consommation nécessite, si disponible,
`glMemoryBarrier` avec bits du domaine **consommateur** (vertex, commandes,
texture…), pas une barrière arbitraire après chaque draw. OpenGL 3.3 core ne
garantit pas ces producteurs compute/SSBO ni cette fonction.

`glFenceSync` + observation `glClientWaitSync` peut protéger le réemploi host ;
`glWaitSync` attend côté serveur. Entre contextes, vérifier flush du producteur,
visibilité/partage et rebinding applicables ; un fence non soumis n'avance pas.
Deletion GL peut différer la destruction interne des objets encore référencés :
cela n'autorise ni l'usage d'un nom supprimé ni un appel sans contexte valide.
Choisir un contexte de destruction compatible ; traiter perte de contexte selon
contrat, pas `glFinish` comme correction permanente.

## Sources et limites

Liens officiels, non copiés ; licence de réutilisation non déduite d'une URL.
Avant toute adaptation externe : fichier, commit exact et notices vérifiés ; un
tree SHA n'est pas un commit. Aucun wrapper ni superviseur tiers importé.

- [Vulkan, synchronization](https://docs.vulkan.org/spec/latest/chapters/synchronization.html).
- [D3D12, resource barriers](https://learn.microsoft.com/en-us/windows/win32/direct3d12/using-resource-barriers-to-synchronize-resource-states-in-direct3d-12).
- [Metal, MTLFence](https://developer.apple.com/documentation/metal/mtlfence).
- [OpenGL, glMemoryBarrier](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glMemoryBarrier.xhtml).
- [OpenGL, glFenceSync](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFenceSync.xhtml).

Sources évolutives : relever version/date pour un correctif. Les pages Apple/GL
peuvent nécessiter un navigateur pour le corps complet ; leur titre accessible
ne prouve pas le contenu relu ni une fonction disponible sur le device.
