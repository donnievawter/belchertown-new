#encoding UTF-8
<div class="entry-content about-outer" itemprop="text">
    <h2>Valle de Stelle</h2>
       <p>To show built in WeeWX variables (like station battery status (if supported), temperature, WeeWX version, etc.), just add them to this file. For example:</p>
    <ul>
        <li>Station hardware: $station.hardware</li>
        <li>Server uptime: $station.os_uptime.long_form</li>
        <li>WeeWX uptime: $station.uptime.long_form</li>
        <li>WeeWX version: $station.version</li>
        <li>New Belchertown Skin Version: $belchertown_version</li>
    </ul>
    <p>For an example of what this page could say, please see <a href="https://belchertownweather.com/about" target="_blank">https://belchertownweather.com/about</a></p>
    <p>Enjoy your new WeeWX skin!</p>
    
    <!-- Frigate Camera Feeds -->
    #if $Extras.has_key("frigate_enabled") and $Extras.frigate_enabled == "1"
    <div class="frigate-camera-section mt-5">
        <h3>Frigate Security Cameras</h3>
        <div class="row g-3" id="frigate-camera-grid">
            <!-- Cameras will be loaded here by JavaScript -->
            <div class="col-12 text-center">
                <div class="spinner-border spinner-border-sm text-secondary" role="status">
                    <span class="visually-hidden">Loading cameras...</span>
                </div>
                <p class="text-muted mt-2">Loading camera feeds...</p>
            </div>
        </div>
    </div>
    
    <script type="text/javascript">
        (function() {
            var frigateRefreshIntervalMs = ($Extras.frigate_refresh_interval_minutes || 2) * 60000;
            var frigateGridElement = document.getElementById('frigate-camera-grid');
            console.log('Frigate module initialized. Refresh interval:', frigateRefreshIntervalMs, 'ms');
            console.log('Frigate config - enabled:', '$Extras.frigate_enabled', 'cameras:', '$Extras.frigate_cameras');
            
            function formatTimestamp(epoch) {
                if (!epoch || typeof dayjs === 'undefined') return '';
                try {
                    return dayjs.unix(epoch).format('LT');
                } catch(e) {
                    return '';
                }
            }
            
            function updateFrigateCameras() {
                if (!frigateGridElement) {
                    console.error('Frigate: camera grid element not found');
                    return;
                }
                
                var url = '/json/frigate_cameras.json?t=' + Date.now();
                console.log('Frigate: Fetching from:', url);
                
                fetch(url, { cache: 'no-store' })
                    .then(function(response) {
                        console.log('Frigate: Fetch response status:', response.status, response.statusText);
                        if (!response.ok) throw new Error('Network response was not ok (' + response.status + ')');
                        return response.json();
                    })
                    .then(function(data) {
                        console.log('Frigate: JSON response received:', data);
                        if (!data.cameras || data.cameras.length === 0) {
                            var errorMsg = 'No cameras available or Frigate API error: ' + (data.error || 'Unknown error');
                            console.warn('Frigate:', errorMsg);
                            frigateGridElement.innerHTML = '<div class="col-12"><div class="alert alert-warning">' + errorMsg + '</div></div>';
                            return;
                        }
                        
                        console.log('Frigate: Found', data.cameras.length, 'cameras');
                        var html = '';
                        data.cameras.forEach(function(camera) {
                            var timestamp = formatTimestamp(camera.timestamp);
                            var timestampHtml = timestamp ? '<small class="text-muted d-block mt-2">Updated: ' + timestamp + '</small>' : '';
                            
                            if (camera.error) {
                                console.warn('Frigate camera error for', camera.name + ':', camera.error);
                                html += '<div class="col-md-4">';
                                html += '  <div class="card h-100">';
                                html += '    <div class="card-body text-center">';
                                html += '      <h5 class="card-title">' + (camera.title || camera.name) + '</h5>';
                                html += '      <div style="background: #f0f0f0; height: 240px; display: flex; align-items: center; justify-content: center; border-radius: 4px;">';
                                html += '        <div>';
                                html += '          <i class="fa fa-camera" style="font-size: 48px; color: #999; margin-bottom: 12px;"></i>';
                                html += '          <p class="text-danger mb-0"><small><strong>Error:</strong> ' + camera.error + '</small></p>';
                                html += '        </div>';
                                html += '      </div>';
                                html += timestampHtml;
                                html += '    </div>';
                                html += '  </div>';
                                html += '</div>';
                            } else {
                                html += '<div class="col-md-4">';
                                html += '  <div class="card h-100">';
                                html += '    <img src="' + camera.url + '?t=' + Date.now() + '" alt="' + camera.name + '" class="card-img-top img-fluid" style="max-height: 300px; object-fit: cover;" loading="lazy">';
                                html += '    <div class="card-body">';
                                html += '      <h5 class="card-title">' + (camera.title || camera.name) + '</h5>';
                                html += timestampHtml;
                                html += '    </div>';
                                html += '  </div>';
                                html += '</div>';
                            }
                        });
                        
                        frigateGridElement.innerHTML = html;
                    })
                    .catch(function(error) {
                        console.error('Frigate: Fetch failed:', error);
                        frigateGridElement.innerHTML = '<div class="col-12"><div class="alert alert-danger">Failed to load camera feeds: ' + error.message + '</div></div>';
                    });
            }
            
            // Load cameras immediately
            updateFrigateCameras();
            
            // Set up periodic refresh
            setInterval(updateFrigateCameras, frigateRefreshIntervalMs);
            
            // Refresh when page becomes visible
            document.addEventListener('visibilitychange', function() {
                if (!document.hidden) {
                    console.log('Frigate: Page became visible, refreshing cameras');
                    updateFrigateCameras();
                }
            });
        })();
    </script>
    #end if
</div>
