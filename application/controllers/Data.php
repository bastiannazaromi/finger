<?php
defined('BASEPATH') OR exit('No direct script access allowed');

class Data extends CI_Controller {
    
    public function ambil_id()
	{
        $this->load->model('M_Data', 'data');

        $rekap = $this->data->ambil_data_terakhir();
        
        if($rekap) // jika ada nilainya ..
        {
            echo $rekap[0]['id_finger'] + 1;
        }
        else
        {
            echo 1;
        }
    }

    public function save()
    {
        $id = $this->input->get('id');

        $data = [
            "id_finger" => $id,
            "nama" => "Default",
            "created" => time()
        ];

        $this->db->insert('tbuser', $data);

        echo "Sukses, id berhasil ditambahkan ke database";
    }

    public function akses()
    {
        $akses = $this->db->get('tbakses')->result_array();

        echo $akses[0]['akses'];
    }

    public function nama()
    {
        $id = $this->input->get('id');

        $this->db->where('id_finger', $id);
        $nama = $this->db->get('tbuser')->result_array();

        echo $nama[0]['nama'];
    }

    public function edit_akses()
    {
        $akses = $this->input->get('akses');

        $data = [
            "akses" => $akses
        ];

        $this->db->update('tbakses', $data);

        echo "Sukses edit akses";
    }

}